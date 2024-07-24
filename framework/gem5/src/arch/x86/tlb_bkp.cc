/*
 * Copyright (c) 2007-2008 The Hewlett-Packard Development Company
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <chrono>
#include <cstring>
#include <memory>

#include "arch/x86/faults.hh"
#include "arch/x86/insts/microldstop.hh"
#include "arch/x86/pagetable_walker.hh"
#include "arch/x86/pseudo_inst_abi.hh"
#include "arch/x86/regs/misc.hh"
#include "arch/x86/regs/msr.hh"
#include "arch/x86/tlb.hh"
#include "arch/x86/x86_traits.hh"
#include "base/trace.hh"
#include "cpu/thread_context.hh"
#include "debug/TLB.hh"
#include "mem/packet_access.hh"
#include "mem/page_table.hh"
#include "mem/request.hh"
#include "sim/full_system.hh"
#include "sim/process.hh"
#include "sim/pseudo_inst.hh"

#define NVM_USER_REG_START 0x120001000
namespace gem5
{
namespace X86ISA {

TLB::TLB(const Params &p)
    : BaseTLB(p), configAddress(0), size(p.size),
      tlb(size), lruSeq(0), m5opRange(p.system->m5opRange()), stats(this)
{
    if (!size)
        fatal("TLBs must have a non-zero size.\n");

    for (int x = 0; x < size; x++) {
        tlb[x].trieHandle = NULL;
        freeList.push_back(&tlb[x]);
    }

    walker = p.walker;
    walker->setTLB(this);
}
/*
Addr
TLB::setupSSP(const RequestPtr &req, TlbEntry *entry, BaseMMU::Mode mode){
    Addr paddr = 0;
    Addr vaddr = req->getVaddr();
    unsigned cacheline = (vaddr>>6)&0x3f;
    assert(cacheline < 64);
    req->set_is_ssp_request(0);
    if (!entry->p1){
        std::cout<<"p1 is zero"<<std::endl;
        return entry->paddr;
    }
    if (entry->updated_bitmap & (1UL<<cacheline)){
        if (entry->current_bitmap & (1UL<<cacheline)){
            paddr = entry->p1;
        }else{
            paddr = entry->paddr;
        }
    }else{
        if (entry->current_bitmap & (1UL<<cacheline)){
            paddr = entry->p1;
        }else{
            paddr = entry->paddr;
        }
        if (mode == BaseMMU::Write){
            entry->updated_bitmap |= (1UL<<cacheline);
            req->set_is_ssp_request(1);
            if (entry->current_bitmap & (1UL<<cacheline)){
                //std::cout<<"modification goes to p0"<<std::endl;
                req->set_P1addr(entry->paddr);
            }else{
                req->set_P1addr(entry->p1);
            }
            entry->current_bitmap ^= (1UL<<cacheline);
        }
    }
    return paddr;
}
*/
void
TLB::evictLRU()
{
    // Find the entry with the lowest (and hence least recently updated)
    // sequence number.

    unsigned lru = 0;
    unsigned long ssp_offset = 0;
    for (unsigned i = 1; i < size; i++) {
        if (tlb[i].lruSeq < tlb[lru].lruSeq)
            lru = i;
    }
    Addr bitmap_address = walker->get_bitmap_address();
    if (bitmap_address >0 && (tlb[lru].paddr >= NVM_USER_REG_START)){
        ssp_offset = ((tlb[lru].paddr&~(0xfff))-NVM_USER_REG_START)>>12;
        struct ssp_entry* temp_entry =
                (struct ssp_entry*)(bitmap_address+
                                (ssp_offset*sizeof(struct ssp_entry)));
        Request::Flags flags = Request::PHYSICAL;
        if (tlb[lru].current_bitmap > 0){
            Addr write_address1 = (Addr)&(temp_entry->current_bitmap);
            RequestPtr request1 = std::make_shared<Request>(
                            write_address1, 8, flags,
                            walker->getrequestorId());
            PacketPtr write1 = new Packet(request1, MemCmd::WriteReq);
            write1->allocate();
            write1->setSSP(1);
            write1->setData((uint8_t*)&(tlb[lru].current_bitmap));
            walker->sendTimingbitmap(write1);
        }
        if (tlb[lru].updated_bitmap>0){
            Addr write_address2 = (Addr)&(temp_entry->updated_bitmap);
            RequestPtr request2 = std::make_shared<Request>(
                            write_address2, 8, flags,
                            walker->getrequestorId());
            PacketPtr write2 = new Packet(request2, MemCmd::WriteReq);
            write2->allocate();
            write2->setData((uint8_t*)&(tlb[lru].updated_bitmap));
            write2->setSSP(1);
            walker->sendTimingbitmap(write2);
            unsigned evicted = 1;
            Addr write_address3 = (Addr)&(temp_entry->evicted);
            RequestPtr request3 = std::make_shared<Request>(
                            write_address3, sizeof(unsigned), flags,
                            walker->getrequestorId());
            PacketPtr write3 = new Packet(request3, MemCmd::WriteReq);
            write3->allocate();
            write3->setData((uint8_t*)&evicted);
            write3->setSSP(1);
            walker->sendTimingbitmap(write3);
        }
    }
    assert(tlb[lru].trieHandle);
    trie.remove(tlb[lru].trieHandle);
    tlb[lru].trieHandle = NULL;
    freeList.push_back(&tlb[lru]);
}

TlbEntry *
TLB::insert(Addr vpn, const TlbEntry &entry)
{
    // If somebody beat us to it, just use that existing entry.
    TlbEntry *newEntry = trie.lookup(vpn);
    if (newEntry) {
        assert(newEntry->vaddr == vpn);
        return newEntry;
    }

    if (freeList.empty())
        evictLRU();

    newEntry = freeList.front();
    freeList.pop_front();

    *newEntry = entry;
    newEntry->lruSeq = nextSeq();
    newEntry->vaddr = vpn;
    newEntry->trieHandle =
    trie.insert(vpn, TlbEntryTrie::MaxBits - entry.logBytes, newEntry);
    return newEntry;
}

TlbEntry *
TLB::lookup(Addr va, bool update_lru)
{
    TlbEntry *entry = trie.lookup(va);
    if (entry && update_lru)
        entry->lruSeq = nextSeq();
    return entry;
}

void
TLB::flushAll()
{
    DPRINTF(TLB, "Invalidating all entries.\n");
    Addr bitmap_address = walker->get_bitmap_address();
    unsigned long ssp_offset = 0;
    Request::Flags flags = Request::PHYSICAL;
    for (unsigned i = 0; i < size; i++) {
        //std::cout<<"i: "<<i<<",ssp offset tlb: "<<ssp_offset<<std::endl;
        if (bitmap_address > 0 && (tlb[i].paddr >= NVM_USER_REG_START)){
            ssp_offset = ((tlb[i].paddr&~(0xfff))-NVM_USER_REG_START)>>12;
            struct ssp_entry* temp_entry =
                    (struct ssp_entry*)(bitmap_address+
                                    (ssp_offset*sizeof(struct ssp_entry)));
            if (tlb[i].current_bitmap > 0){
                //std::cout<<"flushall current bitmap write"<<std::endl;
                Addr write_address1 = (Addr)&(temp_entry->current_bitmap);
                RequestPtr request1 = std::make_shared<Request>(
                                write_address1, 8, flags,
                                walker->getrequestorId());
                PacketPtr write1 = new Packet(request1, MemCmd::WriteReq);
                write1->allocate();
                write1->setData((uint8_t*)&(tlb[i].current_bitmap));
                write1->setSSP(1);
                walker->sendTimingbitmap(write1);
            }
            if (tlb[i].updated_bitmap > 0){
                //std::cout<<"flushall update bitmap write"<<std::endl;
                Addr write_address2 = (Addr)&(temp_entry->updated_bitmap);
                RequestPtr request2 = std::make_shared<Request>(
                                write_address2, 8, flags,
                                walker->getrequestorId());
                PacketPtr write2 = new Packet(request2, MemCmd::WriteReq);
                write2->allocate();
                write2->setData((uint8_t*)&(tlb[i].updated_bitmap));
                write2->setSSP(1);
                walker->sendTimingbitmap(write2);
                unsigned evicted = 1;
                Addr write_address3 = (Addr)&(temp_entry->evicted);
                RequestPtr request3 = std::make_shared<Request>(
                                write_address3, sizeof(unsigned), flags,
                                walker->getrequestorId());
                PacketPtr write3 = new Packet(request3, MemCmd::WriteReq);
                write3->allocate();
                write3->setData((uint8_t*)&evicted);
                write3->setSSP(1);
                walker->sendTimingbitmap(write3);
            }
        }
        if (tlb[i].trieHandle) {
            trie.remove(tlb[i].trieHandle);
            tlb[i].trieHandle = NULL;
            freeList.push_back(&tlb[i]);
        }
    }
}

void
TLB::setConfigAddress(uint32_t addr)
{
    configAddress = addr;
}

void
TLB::flushNonGlobal()
{
    DPRINTF(TLB, "Invalidating all non global entries.\n");
    Addr bitmap_address = walker->get_bitmap_address();
    Request::Flags flags = Request::PHYSICAL;
    unsigned long ssp_offset = 0;
    for (unsigned i = 0; i < size; i++) {
        if (bitmap_address >0 && (tlb[i].paddr >= NVM_USER_REG_START)){
            ssp_offset = ((tlb[i].paddr&~(0xfff))-NVM_USER_REG_START)>>12;
            struct ssp_entry* temp_entry =
                    (struct ssp_entry*)(bitmap_address+
                                    (ssp_offset*sizeof(struct ssp_entry)));
            if (tlb[i].current_bitmap > 0){
                Addr write_address1 = (Addr)&(temp_entry->current_bitmap);
                //assert(sizeof(temp_entry->current_bitmap) == 8);
                RequestPtr request1 = std::make_shared<Request>(
                                write_address1, 8, flags,
                                walker->getrequestorId());
                PacketPtr write1 = new Packet(request1, MemCmd::WriteReq);
                write1->allocate();
                write1->setData((uint8_t*)&(tlb[i].current_bitmap));
                write1->setSSP(1);
                walker->sendTimingbitmap(write1);
            }
            if (tlb[i].updated_bitmap > 0){
                Addr write_address2 = (Addr)&(temp_entry->updated_bitmap);
                //assert(sizeof(temp_entry->updated_bitmap) == 8);
                RequestPtr request2 = std::make_shared<Request>(
                                write_address2, 8, flags,
                                walker->getrequestorId());
                PacketPtr write2 = new Packet(request2, MemCmd::WriteReq);
                write2->allocate();
                write2->setData((uint8_t*)&(tlb[i].updated_bitmap));
                write2->setSSP(1);
                walker->sendTimingbitmap(write2);
                unsigned evicted = 1;
                Addr write_address3 = (Addr)&(temp_entry->evicted);
                //assert(sizeof(temp_entry->evicted) == 4);
                RequestPtr request3 = std::make_shared<Request>(
                                write_address3, sizeof(unsigned), flags,
                                walker->getrequestorId());
                PacketPtr write3 = new Packet(request3, MemCmd::WriteReq);
                write3->allocate();
                write3->setData((uint8_t*)&evicted);
                write3->setSSP(1);
                walker->sendTimingbitmap(write3);
            }
        }
        if (tlb[i].trieHandle && !tlb[i].global) {
            trie.remove(tlb[i].trieHandle);
            tlb[i].trieHandle = NULL;
            freeList.push_back(&tlb[i]);
        }
    }
}

void
TLB::demapPage(Addr va, uint64_t asn)
{
    TlbEntry *entry = trie.lookup(va);
    if (entry) {
        Addr bitmap_address = walker->get_bitmap_address();
        unsigned long ssp_offset = 0;
        if (bitmap_address >0 && (entry->paddr >= NVM_USER_REG_START)){
            ssp_offset = ((entry->paddr&~(0xfff))-NVM_USER_REG_START)>>12;
            struct ssp_entry* temp_entry =
                    (struct ssp_entry*)(bitmap_address+
                                    (ssp_offset*sizeof(struct ssp_entry)));
            Request::Flags flags = Request::PHYSICAL;
            if (entry->current_bitmap > 0){
                Addr write_address1 = (Addr)&(temp_entry->current_bitmap);
                RequestPtr request1 = std::make_shared<Request>(
                                write_address1, 8, flags,
                                walker->getrequestorId());
                PacketPtr write1 = new Packet(request1, MemCmd::WriteReq);
                write1->allocate();
                write1->setData((uint8_t*)&(entry->current_bitmap));
                write1->setSSP(1);
                walker->sendTimingbitmap(write1);
            }
            if (entry->updated_bitmap>0){
                //std::cout<<"demapPage: "<<std::hex<<entry->paddr<<std::endl;
                Addr write_address2 = (Addr)&(temp_entry->updated_bitmap);
                RequestPtr request2 = std::make_shared<Request>(
                                write_address2, 8, flags,
                                walker->getrequestorId());
                PacketPtr write2 = new Packet(request2, MemCmd::WriteReq);
                write2->allocate();
                write2->setData((uint8_t*)&(entry->updated_bitmap));
                write2->setSSP(1);
                walker->sendTimingbitmap(write2);
                unsigned evicted = 1;
                Addr write_address3 = (Addr)&(temp_entry->evicted);
                RequestPtr request3 = std::make_shared<Request>(
                                write_address3, sizeof(unsigned), flags,
                                walker->getrequestorId());
                PacketPtr write3 = new Packet(request3, MemCmd::WriteReq);
                write3->allocate();
                write3->setData((uint8_t*)&evicted);
                write3->setSSP(1);
                walker->sendTimingbitmap(write3);
            }
        }
        trie.remove(entry->trieHandle);
        entry->trieHandle = NULL;
        freeList.push_back(entry);
    }
}

namespace
{

Cycles
localMiscRegAccess(bool read, MiscRegIndex regNum,
                   ThreadContext *tc, PacketPtr pkt)
{
    if (read) {
        RegVal data = htole(tc->readMiscReg(regNum));
        assert(pkt->getSize() <= sizeof(RegVal));
        pkt->setData((uint8_t *)&data);
    } else {
        RegVal data = htole(tc->readMiscRegNoEffect(regNum));
        assert(pkt->getSize() <= sizeof(RegVal));
        pkt->writeData((uint8_t *)&data);
        tc->setMiscReg(regNum, letoh(data));
    }
    return Cycles(1);
}

} // anonymous namespace

Fault
TLB::translateInt(bool read, RequestPtr req, ThreadContext *tc)
{
    DPRINTF(TLB, "Addresses references internal memory.\n");
    Addr vaddr = req->getVaddr();
    Addr prefix = (vaddr >> 3) & IntAddrPrefixMask;
    if (prefix == IntAddrPrefixCPUID) {
        panic("CPUID memory space not yet implemented!\n");
    } else if (prefix == IntAddrPrefixMSR) {
        vaddr = (vaddr >> 3) & ~IntAddrPrefixMask;

        MiscRegIndex regNum;
        if (!msrAddrToIndex(regNum, vaddr))
            return std::make_shared<GeneralProtection>(0);

        req->setPaddr(req->getVaddr());
        req->setLocalAccessor(
            [read,regNum](ThreadContext *tc, PacketPtr pkt)
            {
                return localMiscRegAccess(read, regNum, tc, pkt);
            }
        );

        return NoFault;
    } else if (prefix == IntAddrPrefixIO) {
        // TODO If CPL > IOPL or in virtual mode, check the I/O permission
        // bitmap in the TSS.

        Addr IOPort = vaddr & ~IntAddrPrefixMask;
        // Make sure the address fits in the expected 16 bit IO address
        // space.
        assert(!(IOPort & ~0xFFFF));
        if (IOPort == 0xCF8 && req->getSize() == 4) {
            req->setPaddr(req->getVaddr());
            req->setLocalAccessor(
                [read](ThreadContext *tc, PacketPtr pkt)
                {
                    return localMiscRegAccess(
                            read, MISCREG_PCI_CONFIG_ADDRESS, tc, pkt);
                }
            );
        } else if ((IOPort & ~mask(2)) == 0xCFC) {
            req->setFlags(Request::UNCACHEABLE | Request::STRICT_ORDER);
            Addr configAddress =
                tc->readMiscRegNoEffect(MISCREG_PCI_CONFIG_ADDRESS);
            if (bits(configAddress, 31, 31)) {
                req->setPaddr(PhysAddrPrefixPciConfig |
                        mbits(configAddress, 30, 2) |
                        (IOPort & mask(2)));
            } else {
                req->setPaddr(PhysAddrPrefixIO | IOPort);
            }
        } else {
            req->setFlags(Request::UNCACHEABLE | Request::STRICT_ORDER);
            req->setPaddr(PhysAddrPrefixIO | IOPort);
        }
        return NoFault;
    } else {
        panic("Access to unrecognized internal address space %#x.\n",
                prefix);
    }
}

Fault
TLB::finalizePhysical(const RequestPtr &req,
                      ThreadContext *tc, BaseMMU::Mode mode) const
{
    Addr paddr = req->getPaddr();

    if (m5opRange.contains(paddr)) {
        req->setFlags(Request::STRICT_ORDER);
        uint8_t func;
        pseudo_inst::decodeAddrOffset(paddr - m5opRange.start(), func);
        req->setLocalAccessor(
            [func, mode](ThreadContext *tc, PacketPtr pkt) -> Cycles
            {
                uint64_t ret;
                pseudo_inst::pseudoInst<X86PseudoInstABI, true>(tc, func, ret);
                if (mode == BaseMMU::Read)
                    pkt->setLE(ret);
                return Cycles(1);
            }
        );
    } else if (FullSystem) {
        // Check for an access to the local APIC
        LocalApicBase localApicBase =
            tc->readMiscRegNoEffect(MISCREG_APIC_BASE);
        AddrRange apicRange(localApicBase.base * PageBytes,
                            (localApicBase.base + 1) * PageBytes);

        if (apicRange.contains(paddr)) {
            // The Intel developer's manuals say the below restrictions apply,
            // but the linux kernel, because of a compiler optimization, breaks
            // them.
            /*
            // Check alignment
            if (paddr & ((32/8) - 1))
                return new GeneralProtection(0);
            // Check access size
            if (req->getSize() != (32/8))
                return new GeneralProtection(0);
            */
            // Force the access to be uncacheable.
            req->setFlags(Request::UNCACHEABLE | Request::STRICT_ORDER);
            req->setPaddr(x86LocalAPICAddress(tc->contextId(),
                                              paddr - apicRange.start()));
        }
    }

    return NoFault;
}

Fault
TLB::translate(const RequestPtr &req,
        ThreadContext *tc, BaseMMU::Translation *translation,
        BaseMMU::Mode mode, bool &delayedResponse, bool timing)
{
    Request::Flags flags = req->getFlags();
    int seg = flags & SegmentFlagMask;
    static uint8_t ssp_flag_start = 0;
    //static uint8_t ssp_flag_end = 0;
    unsigned long ssp_offset = 0;
    Addr bitmap_address = walker->get_bitmap_address();
    bool storeCheck = flags & Request::READ_MODIFY_WRITE;
    uint16_t tracking_log_gran = tc->readMiscRegNoEffect(\
                    gem5::X86ISA::MISCREG_LOG_TRACK_GRAN);
    Addr addr_start = (Addr)tc->readMiscRegNoEffect(\
                    gem5::X86ISA::MISCREG_TRACK_START);
    Addr addr_end = (Addr)tc->readMiscRegNoEffect(\
                    gem5::X86ISA::MISCREG_TRACK_END);

    delayedResponse = false;
    // If this is true, we're dealing with a request to a non-memory address
    // space.
    if (seg == SEGMENT_REG_MS) {
        return translateInt(mode == BaseMMU::Read, req, tc);
    }

    Addr vaddr = req->getVaddr();
    DPRINTF(TLB, "Translating vaddr %#x.\n", vaddr);
    if (tracking_log_gran == 0 &&
                    (bitmap_address <= vaddr &&
                    vaddr<(bitmap_address+64)) && !ssp_flag_start){
        //std::cout<<"bitmap aread read"<<std::endl;
        ssp_flag_start = 1;
        for (int i = 1; i < size; i++){
            if ((tlb[i].paddr >= NVM_USER_REG_START)){
                ssp_offset = ((tlb[i].paddr&~(0xfff))-NVM_USER_REG_START)>>12;
                struct ssp_entry* temp_entry = (struct ssp_entry*)(
                                bitmap_address+
                            (ssp_offset*sizeof(struct ssp_entry)));
                Request::Flags flags = Request::PHYSICAL;
                //unsigned evicted = 3;
                Addr read_address = (Addr)&(temp_entry->evicted);
                RequestPtr request1 = std::make_shared<Request>(read_address,
                        sizeof(unsigned), flags, walker->getrequestorId());
                PacketPtr read = new Packet(request1, MemCmd::ReadReq);
                read->allocate();
                //write1->setData((uint8_t*)&evicted);
                read->setSSP(1);
                walker->sendTimingbitmap(read);
            }
        }
    }

    HandyM5Reg m5Reg = tc->readMiscRegNoEffect(MISCREG_M5_REG);

    // If protected mode has been enabled...
    if (m5Reg.prot) {
        DPRINTF(TLB, "In protected mode.\n");
        // If we're not in 64-bit mode, do protection/limit checks
        if (m5Reg.mode != LongMode) {
            DPRINTF(TLB, "Not in long mode. Checking segment protection.\n");
            // Check for a NULL segment selector.
            if (!(seg == SEGMENT_REG_TSG || seg == SYS_SEGMENT_REG_IDTR ||
                        seg == SEGMENT_REG_HS || seg == SEGMENT_REG_LS)
                    && !tc->readMiscRegNoEffect(MISCREG_SEG_SEL(seg)))
                return std::make_shared<GeneralProtection>(0);
            bool expandDown = false;
            SegAttr attr = tc->readMiscRegNoEffect(MISCREG_SEG_ATTR(seg));
            if (seg >= SEGMENT_REG_ES && seg <= SEGMENT_REG_HS) {
                if (!attr.writable && (mode == BaseMMU::Write || storeCheck))
                    return std::make_shared<GeneralProtection>(0);
                if (!attr.readable && mode == BaseMMU::Read)
                    return std::make_shared<GeneralProtection>(0);
                expandDown = attr.expandDown;

            }
            Addr base = tc->readMiscRegNoEffect(MISCREG_SEG_BASE(seg));
            Addr limit = tc->readMiscRegNoEffect(MISCREG_SEG_LIMIT(seg));
            bool sizeOverride = (flags & (AddrSizeFlagBit << FlagShift));
            unsigned logSize = sizeOverride ? (unsigned)m5Reg.altAddr
                                            : (unsigned)m5Reg.defAddr;
            int size = (1 << logSize) * 8;
            Addr offset = bits(vaddr - base, size - 1, 0);
            Addr endOffset = offset + req->getSize() - 1;
            if (expandDown) {
                DPRINTF(TLB, "Checking an expand down segment.\n");
                warn_once("Expand down segments are untested.\n");
                if (offset <= limit || endOffset <= limit)
                    return std::make_shared<GeneralProtection>(0);
            } else {
                if (offset > limit || endOffset > limit)
                    return std::make_shared<GeneralProtection>(0);
            }
        }
        if (m5Reg.submode != SixtyFourBitMode ||
                (flags & (AddrSizeFlagBit << FlagShift)))
            vaddr &= mask(32);
        // If paging is enabled, do the translation.
        if (m5Reg.paging) {
            DPRINTF(TLB, "Paging enabled.\n");
            // The vaddr already has the segment base applied.
            TlbEntry *entry = lookup(vaddr);
            if (mode == BaseMMU::Read) {
                stats.rdAccesses++;
            } else {
                stats.wrAccesses++;
            }
            if (!entry) {
                DPRINTF(TLB, "Handling a TLB miss for "
                        "address %#x at pc %#x.\n",
                        vaddr, tc->pcState().instAddr());
                if (mode == BaseMMU::Read) {
                    stats.rdMisses++;
                } else {
                    stats.wrMisses++;
                }
                if (FullSystem) {
                    Fault fault = walker->start(tc, translation, req, mode);
                    if (timing || fault != NoFault) {
                        // This gets ignored in atomic mode.
                        delayedResponse = true;
                        return fault;
                    }
                    entry = lookup(vaddr);
                    assert(entry);
                } else {
                    Process *p = tc->getProcessPtr();
                    const EmulationPageTable::Entry *pte =
                        p->pTable->lookup(vaddr);
                    if (!pte) {
                        return std::make_shared<PageFault>(vaddr, true, mode,
                                                           true, false);
                    } else {
                        Addr alignedVaddr = p->pTable->pageAlign(vaddr);
                        DPRINTF(TLB, "Mapping %#x to %#x\n", alignedVaddr,
                                pte->paddr);
                        entry = insert(alignedVaddr, TlbEntry(
                                p->pTable->pid(), alignedVaddr, pte->paddr,
                                pte->flags & EmulationPageTable::Uncacheable,
                                pte->flags & EmulationPageTable::ReadOnly));
                    }
                    DPRINTF(TLB, "Miss was serviced.\n");
                }
            }

            DPRINTF(TLB, "Entry found with paddr %#x, "
                    "doing protection checks.\n", entry->paddr);
            // Do paging protection checks.
            bool inUser = (m5Reg.cpl == 3 &&
                    !(flags & (CPL0FlagBit << FlagShift)));
            CR0 cr0 = tc->readMiscRegNoEffect(MISCREG_CR0);
            bool badWrite = (!entry->writable && (inUser || cr0.wp));
            if ((inUser && !entry->user) ||
                (mode == BaseMMU::Write && badWrite)) {
                // The page must have been present to get into the TLB in
                // the first place. We'll assume the reserved bits are
                // fine even though we're not checking them.
                return std::make_shared<PageFault>(vaddr, true, mode, inUser,
                                                   false);
            }
            if (storeCheck && badWrite) {
                // This would fault if this were a write, so return a page
                // fault that reflects that happening.
                return std::make_shared<PageFault>(
                    vaddr, true, BaseMMU::Write, inUser, false);
            }
            Addr paddr = entry->paddr | (vaddr & mask(entry->logBytes));
            //SSP changes
            if (bitmap_address > 0 && (vaddr>addr_start && vaddr<addr_end) &&
                            (entry->paddr >= NVM_USER_REG_START)){
                //std::cout<<"vaddr: "<<std::hex<<vaddr<<std::endl;
                //std::cout<<"start: "<<std::hex<<addr_start<<std::endl;
                //std::cout<<"end: "<<std::hex<<addr_end<<std::endl;
                Addr ssp_paddr = setupSSP(req,entry,mode);
                paddr = ssp_paddr | (vaddr & mask(entry->logBytes));
            }

            if (bitmap_address <= vaddr && vaddr<(bitmap_address+64)){
                //std::cout<<"vaddr: "<<std::hex<<vaddr<<std::endl;
                if (tracking_log_gran >= 1){
                    ssp_flag_start = 0;
                    tc->setMiscRegNoEffect(
                                     gem5::X86ISA::MISCREG_TRACK_SYNC,
                                  (uint64_t)0);
                }else{
                    if (walker->ssp_packet_send ==\
                                 walker->ssp_packet_received){
                     tc->setMiscRegNoEffect(
                                     gem5::X86ISA::MISCREG_TRACK_SYNC,
                                  (uint64_t)1);
                    }
                }
            }
            DPRINTF(TLB, "Translated %#x -> %#x.\n", vaddr, paddr);
            req->setPaddr(paddr);
            if (entry->uncacheable)
                req->setFlags(Request::UNCACHEABLE | Request::STRICT_ORDER);
        } else {
            //Use the address which already has segmentation applied.
            DPRINTF(TLB, "Paging disabled.\n");
            DPRINTF(TLB, "Translated %#x -> %#x.\n", vaddr, vaddr);
            req->setPaddr(vaddr);
        }
    } else {
        // Real mode
        DPRINTF(TLB, "In real mode.\n");
        DPRINTF(TLB, "Translated %#x -> %#x.\n", vaddr, vaddr);
        req->setPaddr(vaddr);
    }

    return finalizePhysical(req, tc, mode);
}

Fault
TLB::translateAtomic(const RequestPtr &req, ThreadContext *tc,
    BaseMMU::Mode mode)
{
    bool delayedResponse;
    return TLB::translate(req, tc, NULL, mode, delayedResponse, false);
}

Fault
TLB::translateFunctional(const RequestPtr &req, ThreadContext *tc,
    BaseMMU::Mode mode)
{
    unsigned logBytes;
    const Addr vaddr = req->getVaddr();
    Addr addr = vaddr;
    Addr paddr = 0;
    if (FullSystem) {
        Fault fault = walker->startFunctional(tc, addr, logBytes, mode);
        if (fault != NoFault)
            return fault;
        paddr = insertBits(addr, logBytes - 1, 0, vaddr);
    } else {
        Process *process = tc->getProcessPtr();
        const auto *pte = process->pTable->lookup(vaddr);

        if (!pte && mode != BaseMMU::Execute) {
            // Check if we just need to grow the stack.
            if (process->fixupFault(vaddr)) {
                // If we did, lookup the entry for the new page.
                pte = process->pTable->lookup(vaddr);
            }
        }

        if (!pte)
            return std::make_shared<PageFault>(vaddr, true, mode, true, false);

        paddr = pte->paddr | process->pTable->pageOffset(vaddr);
    }
    DPRINTF(TLB, "Translated (functional) %#x -> %#x.\n", vaddr, paddr);
    req->setPaddr(paddr);
    return NoFault;
}

void
TLB::translateTiming(const RequestPtr &req, ThreadContext *tc,
    BaseMMU::Translation *translation, BaseMMU::Mode mode)
{
    bool delayedResponse;
    assert(translation);
    Fault fault =
        TLB::translate(req, tc, translation, mode, delayedResponse, true);
    if (!delayedResponse)
        translation->finish(fault, req, tc, mode);
    else
        translation->markDelayed();
}

Walker *
TLB::getWalker()
{
    return walker;
}

TLB::TlbStats::TlbStats(statistics::Group *parent)
  : statistics::Group(parent),
    ADD_STAT(rdAccesses, statistics::units::Count::get(),
             "TLB accesses on read requests"),
    ADD_STAT(wrAccesses, statistics::units::Count::get(),
             "TLB accesses on write requests"),
    ADD_STAT(rdMisses, statistics::units::Count::get(),
             "TLB misses on read requests"),
    ADD_STAT(wrMisses, statistics::units::Count::get(),
             "TLB misses on write requests")
{
}

void
TLB::serialize(CheckpointOut &cp) const
{
    // Only store the entries in use.
    uint32_t _size = size - freeList.size();
    SERIALIZE_SCALAR(_size);
    SERIALIZE_SCALAR(lruSeq);

    uint32_t _count = 0;
    for (uint32_t x = 0; x < size; x++) {
        if (tlb[x].trieHandle != NULL)
            tlb[x].serializeSection(cp, csprintf("Entry%d", _count++));
    }
}

void
TLB::unserialize(CheckpointIn &cp)
{
    // Do not allow to restore with a smaller tlb.
    uint32_t _size;
    UNSERIALIZE_SCALAR(_size);
    if (_size > size) {
        fatal("TLB size less than the one in checkpoint!");
    }

    UNSERIALIZE_SCALAR(lruSeq);

    for (uint32_t x = 0; x < _size; x++) {
        TlbEntry *newEntry = freeList.front();
        freeList.pop_front();

        newEntry->unserializeSection(cp, csprintf("Entry%d", x));
        newEntry->trieHandle = trie.insert(newEntry->vaddr,
            TlbEntryTrie::MaxBits - newEntry->logBytes, newEntry);
    }
}

Port *
TLB::getTableWalkerPort()
{
    return &walker->getPort("port");
}

} // namespace X86ISA
} // namespace gem5
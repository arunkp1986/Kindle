
# parsetab.py
# This file is automatically generated. Do not edit.
_tabversion = '3.2'

_lr_method = 'LALR'

_lr_signature = b'@\xbcf_\xec\xc3E\xd1\x93\x8fn7C\xf6V\xda'
    
_lr_action_items = {'$end':([0,1,2,3,4,5,6,8,14,40,45,],[-2,0,-1,-3,-4,-6,-7,-5,-8,-10,-9,]),'DEF':([0,3,4,5,6,8,14,40,45,],[7,7,-4,-6,-7,-5,-8,-10,-9,]),'ROM':([7,],[9,]),'MACROOP':([7,],[10,]),'LBRACE':([9,13,],[12,12,]),'ID':([10,12,15,16,20,22,23,24,25,28,29,30,31,33,34,36,43,],[13,21,21,-12,32,37,-24,38,39,-13,-14,-18,-19,-25,-26,-27,-28,]),'SEMI':([11,12,15,16,17,18,19,21,26,27,28,29,30,31,32,35,37,41,42,44,],[14,-17,-17,-12,31,-15,-16,-21,40,-11,-13,-14,-18,-19,-20,-23,-29,-22,-30,45,]),'NEWLINE':([12,15,16,17,18,19,20,21,23,28,29,30,31,32,33,34,35,36,37,41,42,43,],[-17,-17,-12,30,-15,-16,34,-21,-24,-13,-14,-18,-19,-20,-25,-26,-23,-27,-29,-22,-30,-28,]),'DOT':([12,15,16,28,29,30,31,],[22,22,-12,-13,-14,-18,-19,]),'EXTERN':([12,15,16,20,23,28,29,30,31,33,34,36,43,],[24,24,-12,24,-24,-13,-14,-18,-19,-25,-26,-27,-28,]),'LPAREN':([13,],[25,]),'RBRACE':([15,16,28,29,30,31,],[27,-12,-13,-14,-18,-19,]),'PARAMS':([21,32,37,],[35,41,42,]),'COLON':([21,32,38,],[36,36,43,]),'RPAREN':([39,],[44,]),}

_lr_action = { }
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = { }
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'file':([0,],[1,]),'opt_rom_or_macros':([0,],[2,]),'rom_or_macros':([0,],[3,]),'rom_or_macro':([0,3,],[4,8,]),'rom_block':([0,3,],[5,5,]),'macroop_def':([0,3,],[6,6,]),'block':([9,13,],[11,26,]),'statements':([12,],[15,]),'statement':([12,15,],[16,28,]),'content_of_statement':([12,15,],[17,17,]),'microop':([12,15,],[18,18,]),'directive':([12,15,],[19,19,]),'labels':([12,15,],[20,20,]),'label':([12,15,20,],[23,23,33,]),'end_of_statement':([17,],[29,]),}

_lr_goto = { }
for _k, _v in _lr_goto_items.items():
   for _x,_y in zip(_v[0],_v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = { }
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> file","S'",1,None,None,None),
  ('file -> opt_rom_or_macros','file',1,'p_file','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',311),
  ('opt_rom_or_macros -> <empty>','opt_rom_or_macros',0,'p_opt_rom_or_macros_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',314),
  ('opt_rom_or_macros -> rom_or_macros','opt_rom_or_macros',1,'p_opt_rom_or_macros_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',317),
  ('rom_or_macros -> rom_or_macro','rom_or_macros',1,'p_rom_or_macros_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',320),
  ('rom_or_macros -> rom_or_macros rom_or_macro','rom_or_macros',2,'p_rom_or_macros_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',323),
  ('rom_or_macro -> rom_block','rom_or_macro',1,'p_rom_or_macro_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',326),
  ('rom_or_macro -> macroop_def','rom_or_macro',1,'p_rom_or_macro_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',327),
  ('rom_block -> DEF ROM block SEMI','rom_block',4,'p_rom_block','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',331),
  ('macroop_def -> DEF MACROOP ID LPAREN ID RPAREN SEMI','macroop_def',7,'p_macroop_def_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',341),
  ('macroop_def -> DEF MACROOP ID block SEMI','macroop_def',5,'p_macroop_def_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',353),
  ('block -> LBRACE statements RBRACE','block',3,'p_block','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',365),
  ('statements -> statement','statements',1,'p_statements_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',371),
  ('statements -> statements statement','statements',2,'p_statements_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',378),
  ('statement -> content_of_statement end_of_statement','statement',2,'p_statement','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',384),
  ('content_of_statement -> microop','content_of_statement',1,'p_content_of_statement_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',389),
  ('content_of_statement -> directive','content_of_statement',1,'p_content_of_statement_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',390),
  ('content_of_statement -> <empty>','content_of_statement',0,'p_content_of_statement_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',395),
  ('end_of_statement -> NEWLINE','end_of_statement',1,'p_end_of_statement','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',400),
  ('end_of_statement -> SEMI','end_of_statement',1,'p_end_of_statement','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',401),
  ('microop -> labels ID','microop',2,'p_microop_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',406),
  ('microop -> ID','microop',1,'p_microop_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',413),
  ('microop -> labels ID PARAMS','microop',3,'p_microop_2','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',419),
  ('microop -> ID PARAMS','microop',2,'p_microop_3','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',427),
  ('labels -> label','labels',1,'p_labels_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',435),
  ('labels -> labels label','labels',2,'p_labels_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',439),
  ('labels -> labels NEWLINE','labels',2,'p_labels_2','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',445),
  ('label -> ID COLON','label',2,'p_label_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',449),
  ('label -> EXTERN ID COLON','label',3,'p_label_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',456),
  ('directive -> DOT ID','directive',2,'p_directive_0','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',464),
  ('directive -> DOT ID PARAMS','directive',3,'p_directive_1','/home/kparun/hybrid_work/create_gemOS_disk_image/Kindle/hscc/gem5/build/X86/arch/micro_asm.py',470),
]

/* impl.c.format: OBJECT FORMATS
 *
 * $HopeName: MMsrc!format.c(MMdevel_telemetry_fmt.1) $
 * Copyright (C) 1997 The Harlequin Group Limited.  All rights reserved.
 *
 * DESIGN
 *
 * See protocol.mps.format.
 */

#include "mpm.h"

SRCID(format, "$HopeName: MMsrc!format.c(MMdevel_telemetry_fmt.1) $");


Bool FormatCheck(Format format)
{
  CHECKS(Format, format);
  CHECKU(Arena, format->arena);
  CHECKL(format->serial < format->arena->formatSerial);
  CHECKL(format->variety == FormatVarietyA || 
	 format->variety == FormatVarietyB);
  CHECKL(RingCheck(&format->arenaRing));
  CHECKL(AlignCheck(format->alignment));
  /* @@@@ alignment should be less than maximum allowed */
  CHECKL(FUNCHECK(format->scan));
  CHECKL(FUNCHECK(format->skip));
  CHECKL(FUNCHECK(format->move));
  CHECKL(FUNCHECK(format->isMoved));
  CHECKL(FUNCHECK(format->copy));
  CHECKL(FUNCHECK(format->pad));
  CHECKL(FUNCHECK(format->class));

  return TRUE;
}

static Addr FormatDefaultClass(Addr object) 
{
  AVER(object != NULL);

  return ((Addr *)object)[0];
}

Res FormatCreate(Format *formatReturn, Arena arena,
                 Align alignment,
		 FormatVariety variety,
                 FormatScanMethod scan,
                 FormatSkipMethod skip,
                 FormatMoveMethod move,
                 FormatIsMovedMethod isMoved,
                 FormatCopyMethod copy,
                 FormatPadMethod pad,
		 FormatClassMethod class)
{
  Format format;
  Res res;
  void *p;

  AVER(formatReturn != NULL);

  res = ArenaAlloc(&p, arena, sizeof(FormatStruct));
  if(res != ResOK)
    return res;
  format = (Format)p; /* avoid pun */

  format->arena = arena;
  RingInit(&format->arenaRing);
  format->alignment = alignment;
  format->variety = variety;
  format->scan = scan;
  format->skip = skip;
  format->move = move;
  format->isMoved = isMoved;
  format->copy = copy;
  format->pad = pad;
  if(class == NULL) {
    AVER(variety == FormatVarietyA); 
    format->class = &FormatDefaultClass;
  } else {
    format->class = class;
  }

  format->sig = FormatSig;
  format->serial = arena->formatSerial;
  ++arena->formatSerial;

  AVERT(Format, format);
  
  RingAppend(&arena->formatRing, &format->arenaRing);

  *formatReturn = format;
  return ResOK;
}


void FormatDestroy(Format format)
{
  AVERT(Format, format);

  RingRemove(&format->arenaRing);

  format->sig = SigInvalid;
  
  RingFinish(&format->arenaRing);

  ArenaFree(format->arena, format, sizeof(FormatStruct));
}

/* Must be thread safe.  See design.mps.interface.c.thread-safety. */
Arena FormatArena(Format format)
{
  /* Can't AVER format as that would not be thread-safe */
  /* AVERT(Format, format); */
  return format->arena;
}


Res FormatDescribe(Format format, mps_lib_FILE *stream)
{
  Res res;
  
  res = WriteF(stream,
               "Format $P ($U) {\n", (WriteFP)format, (WriteFU)format->serial,
               "  arena $P ($U)\n", 
               (WriteFP)format->arena, (WriteFU)format->arena->serial,
               "  alignment $W\n", (WriteFW)format->alignment,
               "  scan $F\n", (WriteFF)format->scan,
               "  skip $F\n", (WriteFF)format->skip,
               "  move $F\n", (WriteFF)format->move,
               "  isMoved $F\n", (WriteFF)format->isMoved,
               "  copy $F\n", (WriteFF)format->copy,
               "  pad $F\n", (WriteFF)format->pad,
               "} Format $P ($U)\n", (WriteFP)format, (WriteFU)format->serial,
               NULL);
  if(res != ResOK) return res;

  return ResOK;
}

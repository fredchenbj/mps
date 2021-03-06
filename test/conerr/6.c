/* 
TEST_HEADER
 id = $Id$
 summary = destroy an arena which contains a root
 language = c
 link = testlib.o
OUTPUT_SPEC
 assert = true
 assertfile P= global.c
 assertcond = RingIsSingle(&arenaGlobals->rootRing)
END_HEADER
*/

#include "testlib.h"
#include "mpscmv.h"

void *stackpointer;

static void test(void)
{
 mps_arena_t arena;
 mps_thr_t thread;
 mps_root_t root;

 cdie(mps_arena_create(&arena, mps_arena_class_vm(), mmqaArenaSIZE), "create arena");
 
 cdie(mps_thread_reg(&thread, arena), "register thread");

 cdie(
  mps_root_create_reg(&root, arena, mps_rank_ambig(), 0, thread,
   mps_stack_scan_ambig, stackpointer, 0), 
  "create root");

 mps_thread_dereg(thread);
 mps_arena_destroy(arena);
 comment("Destroy arena.");
}

int main(void)
{
 void *m;
 stackpointer=&m; /* hack to get stack pointer */

 easy_tramp(test);
 return 0;
}

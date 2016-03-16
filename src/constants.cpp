#include "common.hpp"
#include "constants.hpp"
#include "DFT.h"

namespace zsoundproc {
	void initConstants(State * state, Module & module) {
		Stack * stack = state->stack;
		stack->setField<int>("FLAG_NO_TEST_RUN", DFT_FLAG_NO_TEST_RUN);
		stack->setField<int>("FLAG_LIGHT_TEST_RUN", DFT_FLAG_LIGHT_TEST_RUN);
		stack->setField<int>("FLAG_HEAVY_TEST_RUN", DFT_FLAG_HEAVY_TEST_RUN);
		stack->setField<int>("FLAG_EXHAUSTIVE_TEST_RUN", DFT_FLAG_EXHAUSTIVE_TEST_RUN);
		stack->setField<int>("FLAG_REAL", DFT_FLAG_REAL);
		stack->setField<int>("FLAG_ALT_REAL", DFT_FLAG_ALT_REAL);
		stack->setField<int>("FLAG_VERBOSE", DFT_FLAG_VERBOSE);
		stack->setField<int>("FLAG_NOBITREVERSAL", DFT_FLAG_NO_BITREVERSAL);
		stack->setField<int>("FLAG_FORCE_RECURSIVE", DFT_FLAG_FORCE_RECURSIVE);
		stack->setField<int>("FLAG_FORCE_COBRA", DFT_FLAG_FORCE_COBRA);
	}
}


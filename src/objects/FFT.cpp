#include "objects/FFT.hpp"
#include "DFT.h"
#include "SIMDBase.h"

namespace zsoundproc {
	FFTCore * FFT::constructor(State & state, bool & managed){
		Stack * stack = state.stack;
		if (stack->is<LUA_TNUMBER>(1)){
			if (stack->is<LUA_TNUMBER>(2)){
				int32_t flags = stack->to<int>(2);
				if ((flags & DFT_FLAG_FORCE_RECURSIVE) && (flags & DFT_FLAG_FORCE_COBRA)){
					state.error("Can't use DFT_FLAG_FORCE_RECURSIVE and DFT_FLAG_FORCE_COBRA at the same time!");
				}
					
				return new FFTCore(stack->to<int>(1), flags);
			}
			else{
				return new FFTCore(stack->to<int>(1));
			}
		}
		else{
			return new FFTCore();
		}
	}

	void FFT::destructor(State & state, FFTCore * object){
		delete object;
	}

	int FFT::getExecutionPlan(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->pushLString(object->writeExecutionPlan());
		return 1;
	}
	int FFT::setExecutionPlan(State & state, FFTCore * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			object->readExecutionPlan(stack->toLString(1));
		}
		return 0;
	}

	int FFT::getInput(State & state, FFTCore * object){
		Stack * stack = state.stack;
		REAL_matrix & matrix = object->inputMatrix;
		stack->newTable(); {
			int j = 1;
			for (REAL_matrix::iterator iterRow = matrix.begin(); iterRow != matrix.end(); iterRow++, j++){
				REAL_row & row = (*iterRow);
				int i = 1;

				stack->push<int>(j);
				stack->newTable(); {
					
					for (REAL_row::iterator iter = row.begin(); iter != row.end(); iter++, i++){
						REAL_element & elm = (*iter);

						stack->push<int>(i);
						stack->newTable(); {
							stack->push<int>(1);
							stack->push<LUA_NUMBER>(elm.real());
							stack->setTable();

							stack->push<int>(2);
							stack->push<LUA_NUMBER>(elm.imag());
							stack->setTable();
						}
						stack->setTable();
					}
					
				}
				stack->setTable();
			}
		}
		return 1;
	}
	int FFT::setInput(State & state, FFTCore * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TTABLE>(1)){
			REAL_matrix & matrix = object->inputMatrix;
			size_t cols = stack->objLen(1);
			size_t matrixLength = matrix.size();

			for (int j = 0; j < ((cols <= matrixLength) ? cols : matrixLength); j++){
				REAL_row & row = matrix[j];

				stack->push<int>(j + 1);
				stack->getTable(1);

				if (stack->is<LUA_TTABLE>()){
					size_t rows = stack->objLen(-1);
					size_t rowLength = row.size();

					for (int i = 0; i < ((rows <= rowLength) ? rows : rowLength); i++){
						REAL_element & elm = row[i];

						stack->push<int>(i + 1);
						stack->getTable(-2);
						if (stack->is<LUA_TTABLE>()){
							stack->push<int>(1);
							stack->getTable(-2);
							elm.real(stack->to<LUA_NUMBER>());
							stack->pop(1);

							stack->push<int>(2);
							stack->getTable(-2);
							elm.imag(stack->to<LUA_NUMBER>());
							stack->pop(1);
						}
						else if (stack->is<LUA_TNUMBER>()){
							elm.real(stack->to<LUA_NUMBER>());
							elm.imag(0);
						}
						stack->pop(1);
					}

				}
				stack->pop(1);
			}
		}
		return 0;
	}

	int FFT::getOutput(State & state, FFTCore * object){
		Stack * stack = state.stack;
		REAL_matrix & matrix = object->outputMatrix;
		stack->newTable(); {
			int j = 1;
			for (REAL_matrix::iterator iterRow = matrix.begin(); iterRow != matrix.end(); iterRow++, j++){
				int i = 1;
				REAL_row & row = (*iterRow);

				stack->push<int>(j);
				stack->newTable(); {

					for (REAL_row::iterator iter = row.begin(); iter != row.end(); iter++, i++){
						REAL_element & elm = (*iter);

						stack->push<int>(i);
						stack->newTable(); {
							stack->push<int>(1);
							stack->push<LUA_NUMBER>(elm.real());
							stack->setTable();

							stack->push<int>(2);
							stack->push<LUA_NUMBER>(elm.imag());
							stack->setTable();
						}
						stack->setTable();
					}
				}
				stack->setTable();
			}
		}
		return 1;
	}

	int FFT::setOutput(State & state, FFTCore * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TTABLE>(1)){
			REAL_matrix & matrix = object->outputMatrix;
			size_t cols = stack->objLen(1);
			size_t matrixLength = matrix.size();

			for (int j = 0; j < ((cols <= matrixLength) ? cols : matrixLength); j++){
				REAL_row & row = matrix[j];

				stack->push<int>(j + 1);
				stack->getTable(1);

				if (stack->is<LUA_TTABLE>()){
					size_t rows = stack->objLen(-1);
					size_t rowLength = row.size();

					for (int i = 0; i < ((rows <= rowLength) ? rows : rowLength); i++){
						REAL_element & elm = row[i];

						stack->push<int>(i + 1);
						stack->getTable(-2);
						if (stack->is<LUA_TTABLE>()){
							stack->push<int>(1);
							stack->getTable(-2);
							elm.real(stack->to<LUA_NUMBER>());
							stack->pop(1);

							stack->push<int>(2);
							stack->getTable(-2);
							elm.imag(stack->to<LUA_NUMBER>());
							stack->pop(1);
						}
						else if (stack->is<LUA_TNUMBER>()){
							elm.real(stack->to<LUA_NUMBER>());
							elm.imag(0);
						}
						stack->pop(1);
					}

				}
				stack->pop(1);
			}
		}
		return 0;
	}

	int FFT::execute(State & state, FFTCore * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TNUMBER>(1)){
			object->execute(stack->to<int>(1));
		}
		else{
			object->execute();
		}
		return 0;
	}

	int FFT::getDataType(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_TYPE));
		return 1;
	}

	int FFT::getMode(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_MODE));
		return 1;
	}

	int FFT::getFFTLength(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_FFT_LENGTH));
		return 1;
	}

	int FFT::getIsRealTransform(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_IS_REAL_TRANSFORM));
		return 1;
	}

	int FFT::getNoBitTraversal(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_NO_BIT_REVERSAL));
		return 1;
	}

	int FFT::getTestRun(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getPlanParam(DFT_PARAMID_TEST_RUN));
		return 1;
	}

	int FFT::getModeMax(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_MODE_MAX));
		return 1;
	}
	int FFT::getTypeAvailability(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_TYPE_AVAILABILITY));
		return 1;
	}
	int FFT::getSizeOfReal(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_SIZE_OF_REAL));
		return 1;
	}
	int FFT::getSizeOfVect(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_SIZE_OF_VECT));
		return 1;
	}
	int FFT::getVectorLen(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_VECTOR_LEN));
		return 1;
	}
	int FFT::getModeAvailability(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getModeParam(SIMDBase_PARAMID_MODE_AVAILABILITY));
		return 1;
	}
	int FFT::getModeName(State & state, FFTCore * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->getMode());
		return 1;
	}

	void initFFT(State * state, Module & module){
		INIT_OBJECT(FFT);
	}
};

/** **************************************************************************
 * IRssa.cpp
 *
 *  Created on: 11-May-2018
 *      Author: amit
 * ***************************************************************************
 * Few implementation points for later reference:
 *   Commands to run the script
 "clang -g -Xclang -disable-O0-optnone -emit-llvm -c benchmarks/test2.c -o benchmarks/test2.bc"
 "opt -loop-unroll -mem2reg -instnamer -load src/libControllerProgramPass.so -ssa  benchmarks/test2.bc -o benchmarks/test2
 "lli benchmarks/test2"
 Note:
 1) Due to the clang flag "-disable-O0-optnone" which optimize the IR instructions and eliminate all instructions
 containing alloca,load and store, thus declaration and counting of variable was not possible in the usual way.
 Moreover, if we remove this flag, then -mem2reg pass did not work and as a result IR does not become SSA (ie no phi node
 is generated by LLVM).
 So, due to the use of -mem2reg and -disable-O0-optnone flags in opt and clang, we have to use the -g flag, the debug flag.
 The use of -g enable @llvm.dbg.value instruction to appear and so we could retrieve all variables that was created.

 2) Todo:: due to our lack of understanding of llvm API, we have used string functions (find/substr) to obtain the variables name.

 Important:(invalidates the point 1)
 3) The mem2reg pass works if the two flags -O and -g i.e., "-O1/2/3 -g" used together. This combination works now
 even if "-disable-O0-optnone" flag is not present in the clang command.

 *
 * * *************************************************************************
 */

#include "IRssa.h"
#include "llvm/IR/IntrinsicInst.h"
#include "DataStructures.h"

#include <list>
#include <iostream>
#include <string>

unsigned int IRssa::ssaLineNo = 0;		//initialize the static variable


std::list<std::pair<unsigned int, std::string> > IRssa::getAllInsts() {
	return all_insts;
}

/*bool IRssa::replaceAllInsts(std::string searchSt, std::string replacingSt){
	for(std::list<std::string>::reverse_iterator rev_it = all_insts.rbegin(); rev_it != all_insts.rend(); ++rev_it){
		if (boost::iequals((*rev_it),searchSt)){	//look for the first matching string
			(*rev_it) = replacingSt;	//std::cout<<"Test (iterator) Found matching!!!\n";
			//break;
			return true;
		}
	}
//	(*it) = replacingSt;	//replacing with the modified smt string
	return false;
}*/

bool IRssa::replaceAllInsts(std::string searchSt, std::string replacingSt){
	for(std::list<std::pair<unsigned int, std::string> >::reverse_iterator rev_it = all_insts.rbegin(); rev_it != all_insts.rend(); ++rev_it){
		if (boost::iequals((*rev_it).second, searchSt)){	//look for the first matching string
			(*rev_it).second = replacingSt;	//std::cout<<"Test (iterator) Found matching!!!\n";
			return true;
		}
	}
	return false;
}

bool IRssa::replacePrevious_ite_toImplication(std::pair<unsigned int, std::string> item){

	for(std::list<std::pair<unsigned int, std::string> >::reverse_iterator rev_it = all_insts.rbegin(); rev_it != all_insts.rend(); ++rev_it){
		if ((*rev_it).first == item.first){	//look for the matching ssaLineNo
			//std::cout<<"Found matching = "<<(*rev_it).second<<" !!!\n";
			std::string oldSSA = (*rev_it).second;
			// ***** Do replacement Logic here *****
			boost::replace_first(oldSSA, "ite","=>");// replace_all() replaces all occurrence at once
			(*rev_it).second = oldSSA;	//replacing with the modified ssa string
			// ***** End of replacement Logic *****
			return true;
		}
	}
	return false;
}

void IRssa::setInst(unsigned int lineNo, std::string st) {
	std::pair<unsigned int, std::string>  ssa_str;
	ssa_str.first = lineNo;
	ssa_str.second = st;
	all_insts.push_back(ssa_str);
}

std::list<std::pair<std::string, std::string> > IRssa::getVariables() {
	return ssaVariables;
}
void IRssa::setVariable(std::string varName, std::string VarType) {
	bool found = false;
	std::list<std::pair<std::string, std::string> >::iterator it;
	for (it = ssaVariables.begin(); it != ssaVariables.end(); it++) {
		if (boost::iequals((*it).first, varName))
			found = true;
	}
	if (!found){ //not a duplicate variable
		std::pair<std::string, std::string> temp;
		temp.first = varName;
		temp.second = VarType;
		ssaVariables.push_back(temp);
	}
}

std::list<variable> IRssa::getInputVariables(){
	return inputVariables;
}
void IRssa::setInputVariable(std::string varName, unsigned int id, std::string VarType){
	bool found = false;
	variable inputVar;
	std::list<variable>::iterator it;
	for (it = inputVariables.begin(); it != inputVariables.end(); it++) {
		if (boost::iequals((*it).varName, varName)){
			found = true;
			break;
		}
	}
	if (!found) { //not a duplicate variable
		inputVar.varName = varName;
		inputVar.varType = VarType;
		inputVar.id = id;
		inputVar.versionNames.push_back(varName);
		inputVariables.push_back(inputVar);
	}
}

std::list<variable> IRssa::getOutputVariables(){
	return outputVariables;
}
void IRssa::setOutputVariable(std::string varName, unsigned int id, std::string VarType) {
	bool found = false;
	variable outputVar;
	std::list<variable>::iterator it;
	for (it = outputVariables.begin(); it != outputVariables.end(); it++) {
		if (boost::iequals((*it).varName, varName)) {
			found = true;
			break;
		}
	}
	if (!found) { //not a duplicate variable
		outputVar.varName = varName;
		outputVar.varType = VarType;
		outputVar.id = id;
		outputVar.versionNames.push_back(varName);
		outputVariables.push_back(outputVar);
	}
}

bool IRssa::findVariable(string var, string &varDataType) {
	bool found = false;
	std::list<std::pair<std::string, std::string> >::iterator it;
	for (it = ssaVariables.begin(); it != ssaVariables.end(); it++) {
		if (boost::iequals((*it).first, var)) {
			found = true;
			varDataType = (*it).second;
		}
	}

	return found;
}







const std::map<std::string, funcDump>& IRssa::getFunctionDump() const {
	return functionDump;
}

void IRssa::setFunctionDump(const std::map<std::string, funcDump>& functionDump) {
	this->functionDump = functionDump;
}

#include <cmath>
#include <functional>
#include "scriptlength.h"

/*
* An AngelScript revision removed the possibility of an application
* accepting both array::length and array::length(), which existing scripts
* do all the time, so we build up this elaborate alternate class that
* pretends to be a uint as best as it can but can also be called
*/
asUINT convertUintAddressToUint(asUINT* address) {
	return *address;
}
bool lengthEquals(asUINT* a, asUINT* b) {
	return *a == *b;
}
int lengthCmp(asUINT* a, asUINT* b) {
	if (*a < *b)
		return -1;
	if (*a > *b)
		return 1;
	return 0;
}
asUINT lengthPow(asUINT* a, asUINT* b) {
	return asUINT(std::pow(*a, *b));
}

#define lengthOperator(name, op) static std::function<asUINT(asUINT*, asUINT*)> length ## name = [](asUINT* a, asUINT* b){\
	return *a op *b;\
};\
engine->RegisterObjectMethod("addon_callable_length", "uint op" # name "(const addon_callable_length &in) const", asMETHOD(decltype(length ## name), operator()), asCALL_THISCALL_OBJFIRST, &(length ## name))
#define lengthShiftOperator(name, op) static std::function<asUINT(asUINT*, asUINT)> lengthShift ## name = [](asUINT* a, asUINT b){\
	return *a op b;\
};\
engine->RegisterObjectMethod("addon_callable_length", "uint op" # name "(const uint) const", asMETHOD(decltype(lengthShift ## name), operator()), asCALL_THISCALL_OBJFIRST, &(lengthShift ## name))

void RegisterScriptAddonCallableLength(asIScriptEngine* engine) {
	engine->RegisterObjectType("addon_callable_length", sizeof(asUINT), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<asUINT>());
	engine->RegisterObjectMethod("addon_callable_length", "uint opCall() const", asFUNCTION(convertUintAddressToUint), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("addon_callable_length", "uint opImplConv() const", asFUNCTION(convertUintAddressToUint), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("addon_callable_length", "bool opEquals(const addon_callable_length &in) const", asFUNCTION(lengthEquals), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("addon_callable_length", "int opCmp(const addon_callable_length &in) const", asFUNCTION(lengthCmp), asCALL_CDECL_OBJFIRST);
	lengthOperator(Add, +);
	lengthOperator(Sub, -);
	lengthOperator(Mul, *);
	lengthOperator(Div, /);
	lengthOperator(Mod, %);
	engine->RegisterObjectMethod("addon_callable_length", "uint opPow(const addon_callable_length &in) const", asFUNCTION(lengthPow), asCALL_CDECL_OBJFIRST);
	lengthOperator(And, &);
	lengthOperator(Or, |);
	lengthOperator(Xor, ^);
	lengthOperator(Shl, <<);
	lengthOperator(Shr, >>);
	lengthOperator(UShr, >>);
	lengthShiftOperator(Shl, <<); //AngelScript seems to have an issue where it doesn't call opImplConv for shift operators, e.g. foo.length + 1 works but not foo.length << 1
	lengthShiftOperator(Shr, >>);
	lengthShiftOperator(UShr, >>);
}

#undef lengthOperator
#undef lengthShiftOperator

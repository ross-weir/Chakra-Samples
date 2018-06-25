#pragma once
#include <string>
#include <map>

#include "ChakraCore.h"

struct moduleJob {
	JsModuleRecord moduleRecord;
	char* source;
	JsSourceContext sourceContext;
	size_t sourceSize;
};

struct module {
	char* filename;
	JsModuleRecord record;
};

JsValueRef CALLBACK JsPrint(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

JsErrorCode CALLBACK FetchImportedModule(JsModuleRecord importer, JsValueRef specifier, JsModuleRecord* outModule);
JsErrorCode CALLBACK FetchDynamicImportedModule(JsModuleRecord importer, JsValueRef specifier, JsModuleRecord* outModule);
JsErrorCode CALLBACK OnNotifyModuleReady(JsModuleRecord module, JsValueRef exception);

std::wstring LoadScript(std::wstring fileName);
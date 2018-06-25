#include <iostream>
#include <windows.h>

#include "Callbacks.h"

// This should be <string, record> string being the specifier (path to file)
std::map<JsModuleRecord, std::string> moduleRecordMap;

JsValueRef CALLBACK JsPrint(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	JsValueType type;
	JsGetValueType(arguments[1], &type);

	JsValueRef str;
	JsConvertValueToString(arguments[1], &str);

	const wchar_t* s;
	size_t len;
	JsStringToPointer(str, &s, &len);

	std::wcout << s << "\n";

	return JS_INVALID_REFERENCE;
}

JsErrorCode CALLBACK FetchImportedModule(JsModuleRecord referencingModule, JsValueRef specifier, JsModuleRecord* dependentModuleRecord)
{
	auto moduleEntry = moduleRecordMap.find(referencingModule);

	if (moduleEntry != moduleRecordMap.end()) {
		// module exists return it
		*dependentModuleRecord = moduleEntry->first;
		return JsNoError;
	}

	// module doesn't exist create and return
	JsModuleRecord moduleRecord;
	JsErrorCode moduleError = JsInitializeModuleRecord(referencingModule, specifier, &moduleRecord);

	moduleRecordMap[moduleRecord] = "Test";
	*dependentModuleRecord = moduleRecord;

	return JsNoError;
}

JsErrorCode CALLBACK FetchDynamicImportedModule(JsModuleRecord importer, JsValueRef specifier, JsModuleRecord* outModule)
{
	return FetchImportedModule(nullptr, specifier, outModule);
}

JsErrorCode CALLBACK OnNotifyModuleReady(JsModuleRecord module, JsValueRef exception)
{
	moduleJob job;

	job.moduleRecord = module;

	return JsNoError;
}

std::wstring LoadScript(std::wstring fileName)
{
	FILE *file;
	if (_wfopen_s(&file, fileName.c_str(), L"rb"))
	{
		fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", fileName.c_str());
		return std::wstring();
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char *rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return std::wstring();
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);

	wchar_t *contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return std::wstring();
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return std::wstring();
	}

	std::wstring result = contents;
	free(rawBytes);
	free(contents);
	return result;
}
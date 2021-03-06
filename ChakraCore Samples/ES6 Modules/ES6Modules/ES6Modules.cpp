#include <iostream>

#include "Callbacks.h"
#include "ChakraCore.h"

int main()
{
	JsRuntimeHandle runtime;
	JsCreateRuntime(JsRuntimeAttributeEnableExperimentalFeatures, nullptr, &runtime);

	JsContextRef context;
	JsCreateContext(runtime, &context);
	JsSetCurrentContext(context);

	JsValueRef global;
	JsGetGlobalObject(&global);

	// Make a util print function to verify imports work
	const char* funcName = "jsPrint";
	JsValueRef nameVar;
	JsValueRef propRef;
	JsCreateString(funcName, strlen(funcName), &nameVar);
	JsCreateNamedFunction(nameVar, JsPrint, nullptr, &propRef);

	JsPropertyIdRef propId;
	JsGetPropertyIdFromName(L"jsPrint", &propId);
	JsSetProperty(global, propId, propRef, false);

	// Create a module record
	JsModuleRecord moduleRecord;
	JsInitializeModuleRecord(nullptr, nullptr, &moduleRecord);
	/*
		FetchImportedModuleCallBack

		This function will be called when a module is being imported from within another module
		the specifier will be supplied; you need to implement an algorithm for checking if a
		record has already been created for the requested module - return it if it has; if there is 
		no record for the requested module you need to create a module record for it and return that
		note this requires you to run your own array or table of some kind to store the module records 
		you've created so you can do this check.
	*/
	JsSetModuleHostInfo(moduleRecord, JsModuleHostInfo_FetchImportedModuleCallback, FetchImportedModule);
	/*
		FetchImportedModuleFromScriptCallBack

		This function will be similar to the above except it will be called for import() from a script
		rather than from a module - the key difference is that it has a source context parameter instead
		of a module record parameter. Warning - the header I reference above has an incorrect comment in 
		describing this function - it describes the next one by mistake...
	*/
	JsSetModuleHostInfo(moduleRecord, JsModuleHostInfo_FetchImportedModuleFromScriptCallback, FetchDynamicImportedModule);
	/*
		NotifyModuleReadyCallback
		
		This function will be called when the module record has been instantiated, it needs to arrange for
		a call to JsParseModuleSource to be made to Parse the Module; and JsModuleEvaluation to evaluate the
		module (only after it and all dependencies have been parsed note you only need to Evaluate the root 
		parent module, don't evaluate the dependencies, just parse them before the evaluate call for their parent)
		these calls should be done from your event loop, so ideally this callback will add the module record to some sort of queue.
	*/
	JsSetModuleHostInfo(moduleRecord, JsModuleHostInfo_NotifyModuleReadyCallback, OnNotifyModuleReady);

	// Start the event loop here

	JsValueRef result;
	unsigned currentSourceContext = 0;
	JsErrorCode err = JsRunScript(LoadScript(L"script.js").c_str(), currentSourceContext++, L"", &result);

	if (err != JsNoError) {
		std::cout << "ES6 Modules :: There was a fatal error in the script.\n";
		std::cout << err << "\n";
	}

	std::cin.get();

	JsSetCurrentContext(JS_INVALID_REFERENCE);
	JsDisposeRuntime(runtime);

    return 0;
}


#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "DexFile.h"
#include "DexClass.h"

#define TAG "DexFileParser"
#define ALOG(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);

#define CLASS_NAME "Lcom/example/hellojni/Render;"
#define METHOD_1 "paintToBlue"
#define METHOD_2 "paintToRed"

/**
 * Created by duanjin on 4/4/15.
 */

//DexFile structure we need to construct
DexFile* pDexFile;

/*
 *read /proc/self/maps to get the memory location of odex file
*/
void* getODexAddrFromFile(char* fileName, int* size) {

    const char* memFile = "/proc/self/maps";

    FILE* file = fopen(memFile, "r");
    if (file == NULL) {
        ALOG("Can not file file %s", memFile);
        return NULL;
    }

    ALOG("In getDexAddrFromFile method");
    char line[1024];
    char* not_use;
    long addr = 0;
    long end = 0;
    while(fgets(line, sizeof(line), file)) {
        ALOG("%s", line);
        if(strstr(line, fileName)) {
            ALOG("Find line: %s", line);
            char* startAddr = strtok(line, "- ");
            char* endAddr = strtok(NULL, "- ");
            ALOG("Start Addr is %s, End Addr is %s", startAddr, endAddr);
            addr = strtoul(startAddr, &not_use, 16);
            end = strtoul(endAddr, &not_use, 16);
            *size = end - addr;
            ALOG("Addr is %s", addr);
            break;
        }
    }
    fclose(file);
    ALOG("out of method");
    return (void*)addr;
}

//get dex base address from odexHeader
void* getDexBaseAddr(void* odexAddr) {
    DexOptHeader* optHeader = (DexOptHeader*)odexAddr;
    return (void*)(optHeader->dexOffset + odexAddr);
}

DexStringId* getDexStringIds(void* dexAddr) {
    DexHeader* header = (DexHeader*)dexAddr;
    return (DexStringId*)(dexAddr + header->stringIdsOff);
}

DexTypeId* getDexTypeIds(void* dexAddr) {
    DexHeader* header = (DexHeader*)dexAddr;
    return (DexTypeId*)(dexAddr + header->typeIdsOff);
}

DexFieldId* getDexFieldIds(void* dexAddr) {
   DexHeader* header = (DexHeader*)dexAddr;
   return (DexFieldId*)(dexAddr + header->fieldIdsOff);
}

DexMethodId* getDexMethodIds(void* dexAddr) {
   DexHeader* header = (DexHeader*)dexAddr;
   return (DexMethodId*)(dexAddr + header->methodIdsOff);
}

DexProtoId* getDexProtoIds(void* dexAddr) {
    DexHeader* header = (DexHeader*)dexAddr;
    return (DexProtoId*)(dexAddr + header->protoIdsOff);
}

DexClassDef* getDexClassDefs(void* dexAddr) {
    DexHeader* header = (DexHeader*)dexAddr;
    return (DexClassDef*)(dexAddr + header->classDefsOff);
}

void initDexFileStructure(void* dexAddr) {
    if(pDexFile == NULL) {
       pDexFile = (DexFile*) malloc(sizeof(DexFile));
    }
     pDexFile->pHeader = (DexHeader*)dexAddr;
     pDexFile->pStringIds = getDexStringIds(dexAddr);
     pDexFile->pTypeIds = getDexTypeIds(dexAddr);
     pDexFile->pFieldIds = getDexFieldIds(dexAddr);
     pDexFile->pMethodIds = getDexMethodIds(dexAddr);
     pDexFile->pProtoIds = getDexProtoIds(dexAddr);
     pDexFile->pClassDefs = getDexClassDefs(dexAddr);
     pDexFile->baseAddr = (u1*)dexAddr;
}

const DexCode* getMethodFromClass(DexClassData* classData, char* targetMethod) {
    for(int i = 0; i < classData->header.virtualMethodsSize; ++i) {
        DexMethod pDexMethod = classData->virtualMethods[i];
        DexMethodId methodId = pDexFile->pMethodIds[pDexMethod.methodIdx];

        u4 result;
        const char* name = dexStringAndSizeById(pDexFile, methodId.nameIdx, &result);
        if(strcmp(name, targetMethod) == 0) {
            const DexCode* pCode = dexGetCode(pDexFile, &pDexMethod);
            return pCode;
        }
    }
    return NULL;
}

void swap(const DexCode* code1, const DexCode* code2) {
      if(code1 != NULL && code2 != NULL) {
        DexCode* lCode = (DexCode*)code1;
        if(lCode != NULL && code2 != NULL) {
            lCode->registersSize = code2->registersSize;
            lCode->insnsSize = code2->insnsSize;
            for(u4 ins = 0; ins < code2->insnsSize; ++ins) {
                lCode->insns[ins] = code2->insns[ins];
            }
        }
      }
}

//helper methods for ppt
const char* getString(int index) {
       u4 result;
       const char* name = dexStringAndSizeById(pDexFile, index, &result);
       return name;
}
void printAllDexStrings() {

    ALOG("Dex Strings number is: %d", pDexFile->pHeader->stringIdsSize);
    for(int i = 0; i < pDexFile->pHeader->stringIdsSize; ++i) {
         u4 result;
         const char* name = dexStringAndSizeById(pDexFile, i, &result);
         ALOG("String: %s", name);
    }
}

void printAllTypeIds() {
    ALOG("Dex Type number is %d", pDexFile->pHeader->typeIdsSize);
    for(int i = 0; i < pDexFile->pHeader->typeIdsSize; ++i) {
        const char* name = dexStringByTypeIdx(pDexFile, i);
         ALOG("Type is %s", name);
    }
}

void printAllProtoTypes() {
    ALOG("Dex Proto types number is %d", pDexFile->pHeader->protoIdsSize);
    for(int i = 0; i < pDexFile->pHeader->protoIdsSize; ++i) {
         const DexProtoId* proId = dexGetProtoId(pDexFile, i);
         const char* name = getString(proId->shortyIdx);
         ALOG("Proto is %s", name);
    }
}

void printAllMethods() {
    ALOG("Dex Method number is %d", pDexFile->pHeader->methodIdsSize);
    for(int i = 0; i < pDexFile->pHeader->methodIdsSize; ++i) {
        const DexMethodId* methodId = dexGetMethodId(pDexFile, i);
        const char* name = getString(methodId->nameIdx);
        ALOG("Method is %s", name);
    }

}

void method_swizzle() {

    int dexFileSize;
    char* fileName = "/data/dalvik-cache/data@app@com.example.hellojni2";
    void* odexAddr = getODexAddrFromFile(fileName, &dexFileSize);

    //init pDexFile structure from odexAddr
    if(odexAddr != NULL) {

        DexOptHeader* optHeader = (DexOptHeader*)odexAddr;
        void* dexAddr = getDexBaseAddr(odexAddr);
        initDexFileStructure(dexAddr);
        ALOG("magic %s", pDexFile->pHeader->magic);

        const u1* pEncodedData = NULL;
        u4 result;
        const DexClassDef* clazz = NULL;
        const char* name = NULL;

        for(int i = 0; i < pDexFile->pHeader->classDefsSize; ++i) {
            clazz = dexGetClassDef(pDexFile, i);
            const DexTypeId classType = pDexFile->pTypeIds[clazz->classIdx];
            name = dexStringAndSizeById(pDexFile, classType.descriptorIdx, &result);
            ALOG("class is %s", name);
            if(strcmp(name, CLASS_NAME ) == 0) {
                pEncodedData = dexGetClassData(pDexFile, clazz);
                DexClassData* classData = dexReadAndVerifyClassData(&pEncodedData, NULL);
                const DexCode* pCode1 = getMethodFromClass(classData, METHOD_1);
                const DexCode* pCode2 = getMethodFromClass(classData, METHOD_2);

                if(mprotect(odexAddr, dexFileSize, PROT_READ | PROT_WRITE | PROT_EXEC) == 0){
                    swap(pCode1, pCode2);
                    mprotect(odexAddr, dexFileSize, PROT_READ | PROT_EXEC);
                }
                break;
            }
        }
    }
}



extern "C"  jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;
    ALOG("in JNI_Onload");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOG("GetEnv failed");
        return result;
    }
    method_swizzle();
    result = JNI_VERSION_1_4;
    ALOG("out JNI_Onload");
    return result;
}

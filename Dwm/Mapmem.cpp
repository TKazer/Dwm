#include "util.h"

BOOL util::MapMem::GetMapMemFunction(PMemory_func func) {
	if (!func) return FALSE;
	DWORD64 Base = (DWORD64)GetModuleHandleA("kernelbase.dll");
	if (((PIMAGE_DOS_HEADER)Base)->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(Base + ((PIMAGE_DOS_HEADER)Base)->e_lfanew);
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)  return FALSE;
	PVOID base = VirtualAlloc(0, pNtHeader->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!base) return FALSE;
	RtlCopyMemory(base, (PVOID)Base, pNtHeader->OptionalHeader.SizeOfImage);
	RtlZeroMemory(base, 0x1000);
	ULONG old = 0;
	VirtualProtect(base, 0x1000, PAGE_NOACCESS, &old);
	VirtualProtect((PVOID)((ULONG64)base + 0x1000), pNtHeader->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READ, &old);
	func->pOpenFileMappingW = (PVOID)((DWORD64)base + (DWORD64)GetProcAddress((HMODULE)Base, "OpenFileMappingW") - Base);
	func->pMapViewOfFileExNuma = (PVOID)((DWORD64)base + (DWORD64)GetProcAddress((HMODULE)Base, "MapViewOfFileExNuma") - Base);
	return TRUE;
}

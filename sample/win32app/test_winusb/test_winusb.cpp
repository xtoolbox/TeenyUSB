#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winusb.h>
#include <usb.h>
#include <iostream>
#include <cfgmgr32.h>
#include <initguid.h>
#include <combaseapi.h>

#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "Winusb.lib")

const TCHAR* get_first_interface_device_path(LPCTSTR guidStr)
{
  static TCHAR device_path[MAX_PATH];
  GUID guid;
  HRESULT hr = CLSIDFromString(guidStr, &guid);
  if (hr != NOERROR) {
    printf("Fail to convert GUID from string");
    return NULL;
  }
  ULONG list_size = 0;

  CONFIGRET cr = CR_SUCCESS;
  cr = CM_Get_Device_Interface_List_Size(&list_size, &guid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

  if (cr != CR_SUCCESS) {
    hr = HRESULT_FROM_WIN32(CM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
    return NULL;
  }

  TCHAR* list = new TCHAR[list_size];
  memset(list, 0, list_size);
  TCHAR* res = NULL;
  cr = CM_Get_Device_Interface_List(&guid, NULL, list, list_size, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
  int str_len;
  TCHAR* temp_str;
  if (cr != CR_SUCCESS) {
    goto error;
  }

  if (list[0] == 0) {
    goto error;
  }
  _tcscpy_s(device_path, list);

  str_len = _tcslen(list);
  temp_str = list;
  while (str_len + 2 < list_size) {
    temp_str += str_len + 1;
    _tprintf(_T("Ignore more device interfaces: %s\n"), temp_str);
    list_size -= str_len + 1;
    str_len = _tcslen(temp_str);
  }

  res = device_path;

error:
  if (list) {
    delete list;
    list = NULL;
  }
  return res;
}

// ensure data size is not multiple of endpoint Max Packet size or we will send a zero length packet
UCHAR test_data_out[256-1];

UCHAR test_data_in[1024];

int main()
{
  const TCHAR* path = get_first_interface_device_path(_T("{1D4B2365-4749-48EA-B38A-7C6FDDDD7E23}"));
  if (path == NULL) {
    printf("device not found\n");
    return 0;
  }
  _tprintf(_T("Got device interface %s\n"), path);


  HANDLE hDev = CreateFile(path,
    GENERIC_WRITE | GENERIC_READ,
    FILE_SHARE_WRITE | FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
    NULL);

  if (INVALID_HANDLE_VALUE == hDev) {

    printf("fail to open device, last error %d\n", GetLastError());
    return 0;
  }

  WINUSB_INTERFACE_HANDLE hWinusb;

  BOOL bResult = WinUsb_Initialize(hDev, &hWinusb);

  if (!bResult) {
    CloseHandle(hDev);
    printf("Fail to invoke WinUsb_Initialize, last error %d\n", GetLastError());
    return 0;
  }

  ULONG transferred;
  for (int i = 0; i<sizeof(test_data_out); i++) {
    test_data_out[i] = i;
  }
  bResult = WinUsb_WritePipe(hWinusb, 0x01, test_data_out, sizeof(test_data_out), &transferred, NULL);
  if (!bResult) {
    printf("Fail to invoke WinUsb_WritePipe, last error %d\n", GetLastError());
    goto error;
  }
  printf("Write %d bytes data\n", transferred);

  bResult = WinUsb_ReadPipe(hWinusb, 0x81, test_data_in, sizeof(test_data_in), &transferred, NULL);
  if (!bResult) {
    printf("Fail to invoke WinUsb_WritePipe, last error %d\n", GetLastError());
    goto error;
  }
  printf("Read back %d bytes data\n", transferred);

error:
  
  WinUsb_Free(hWinusb);
  CloseHandle(hDev);

  exit(0);
}


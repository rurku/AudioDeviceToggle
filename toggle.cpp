// toggle.cpp : main project file.

#include "stdafx.h"
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "SetDefaultAudioPlaybackDevice.h"
using namespace System;
using namespace System::Collections::Generic;

void ThrowIfError(HRESULT hr, String^ message)
{
	if (!SUCCEEDED(hr))
	{
		throw gcnew Exception(String::Format("HRESULT: {0} - {1}", hr, message));
	}
}



int main(array<System::String ^> ^args)
{
	array<String^>^ favouriteDevices = args;// { "Siberia 800 Rear Panel Optical", "Njoy Speaker" };

	IMMDeviceEnumerator* pEnumerator;
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	HRESULT hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);

	ThrowIfError(hr, "CoCreateInstance");

	IMMDeviceCollection* pDeviceCollection;
	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
	ThrowIfError(hr, "EnumAudioEndpoints");

	UINT deviceCount;
	hr = pDeviceCollection->GetCount(&deviceCount);
	ThrowIfError(hr, "GetCount");

	IMMDevice* defaultDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &defaultDevice);
	ThrowIfError(hr, "GetDefaultAudioEndpoint");

	IPropertyStore *pStore;
	hr = defaultDevice->OpenPropertyStore(STGM_READ, &pStore);
	ThrowIfError(hr, "OpenPropertyStore");

	PROPVARIANT pv;
	hr = pStore->GetValue(PKEY_Device_DeviceDesc, &pv);
	ThrowIfError(hr, "GetValue");

	String^ defaultDeviceName = gcnew String(pv.pwszVal);
	Console::WriteLine("Default device: {0}", defaultDeviceName);
	for (int i = 0; i < deviceCount; i++)
	{
		IMMDevice* pDevice;
		hr = pDeviceCollection->Item(i, &pDevice);
		ThrowIfError(hr, "Item " + i);

		LPWSTR strId;
		hr = pDevice->GetId(&strId);
		ThrowIfError(hr, "GetId");

		DWORD state;
		hr = pDevice->GetState(&state);
		ThrowIfError(hr, "GetState");

		IPropertyStore *pStore;
		hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
		ThrowIfError(hr, "OpenPropertyStore");

		PROPVARIANT pv;
		hr = pStore->GetValue(PKEY_Device_DeviceDesc, &pv);
		ThrowIfError(hr, "GetValue");

		Console::WriteLine(gcnew String(pv.pwszVal));
		String^ devName = gcnew String(pv.pwszVal);
		if (((IList<String^>^)favouriteDevices)->Contains(devName) && devName != defaultDeviceName)
		{
			hr = SetDefaultAudioPlaybackDevice(strId);
			break;
		}
	}
}

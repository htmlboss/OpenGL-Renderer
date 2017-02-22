/*
 * Copyright 2014  NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.
 *
 * This software and the information contained herein is PROPRIETARY and
 * CONFIDENTIAL to NVIDIA and is being provided under the terms and conditions
 * of a form of NVIDIA software license agreement.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef _NVPMAPI_INTERFACEMANAGER_WIN_H_
#define _NVPMAPI_INTERFACEMANAGER_WIN_H_

#include <windows.h>
#include <assert.h>
#include "NvPmApi.h"

//  For lifetime manager of NVPMAPI DLL
template<class EXPORTED_API> class InterfaceManager
{
public:
    InterfaceManager(NVPM_UUID InterfaceUUID) : m_hModule(0), m_InterfaceUUID(InterfaceUUID)
    {
        ClearApiTables();
    }

    ~InterfaceManager()
    {
        if (m_hModule)
        {
            ClearApiTables();
            FreeLibrary(m_hModule);
            m_hModule = 0;
        }
    }

    NVPMRESULT Construct(const wchar_t* pLibraryPath)
    {
        if(pLibraryPath == NULL)
        {
            return NVPM_ERROR_INVALID_PARAMETER;
        }

        if (m_hModule != NULL)
        {
            return NVPM_WARNING_DUPLICATE;
        }

        if((m_hModule = LoadLibraryW(pLibraryPath)) == NULL)
        {
            return NVPM_FAILURE;
        }

        NVPMGetExportTable_Pfn pfnGetExportTable = (NVPMGetExportTable_Pfn)GetProcAddress(m_hModule, "NVPMGetExportTable");

        if(pfnGetExportTable == NULL)
        {
            return NVPM_FAILURE;
        }

        if(pfnGetExportTable(&m_InterfaceUUID, (void **) &m_pNvPmApi) != NVPM_OK)
        {
            return NVPM_FAILURE;
        }

        if(m_pNvPmApi->struct_size < sizeof(EXPORTED_API))
        {
            return NVPM_FAILURE;
        }

        return NVPM_OK;
    }

    const EXPORTED_API* Api() const
    {
        return m_pNvPmApi;
    }

private:
    HMODULE m_hModule;
    const EXPORTED_API *m_pNvPmApi;
    NVPM_UUID m_InterfaceUUID;

private:
    void ClearApiTables()
    {
        m_pNvPmApi = NULL;
    }
};

#endif

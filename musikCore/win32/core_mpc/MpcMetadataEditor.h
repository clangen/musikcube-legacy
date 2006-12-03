#pragma once

#include <string>
#include <fstream>
#include <mpcdec/mpcdec.h>

class MpcMetadataEditor
{
public:

    MpcMetadataEditor(const wchar_t* filename);
    ~MpcMetadataEditor();

    bool IsValid(){ throw; }

    bool Write();

private:

    std::wstring filename;
};
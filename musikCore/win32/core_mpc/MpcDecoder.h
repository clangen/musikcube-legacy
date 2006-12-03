///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#pragma once

#include <iostream>
#include "MpcMetadataEditor.h"

class MpcDecoder
{
    friend mpc_int32_t read_impl(void *data, void *ptr, mpc_int32_t size);
    friend mpc_bool_t seek_impl(void *data, mpc_int32_t offset);
    friend mpc_int32_t tell_impl(void *data);
    friend mpc_int32_t get_size_impl(void *data);
    friend mpc_bool_t canseek_impl(void *data);

public:

    MpcDecoder(const wchar_t* filename);
    ~MpcDecoder();

    bool CanPlay();
    bool CanSeek();
    int GetDuration();
    int GetTime();
    bool Seek(int ms);

    int GetSampleRate();
    int GetNumChannels();

    bool IsValid(){ return this->isValid; }

    unsigned int Decode(void* buffer, unsigned int length);

private:

    class Buffer
    {
    public:
        Buffer(mpc_decoder* decoder, unsigned int bps, unsigned int numChannels, mpc_uint32_t streamLength);
        unsigned int Read(short* target, unsigned int length);

    private:

        void BufferNext();

        MPC_SAMPLE_FORMAT buffer[MPC_DECODER_BUFFER_LENGTH];
        mpc_decoder* decoder;
        unsigned int bufferLength;
        unsigned int bufferOffset;
        mpc_uint32_t streamLength;
        unsigned int bps;
        unsigned int numChannels;
        int floatScale;
    };

    Buffer* buffer;

    mpc_reader_file readerFile;
    mpc_streaminfo streamInfo;
    mpc_decoder decoder;

    FILE* stream;
    unsigned int durationMS;

    bool isValid;
};
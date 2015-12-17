#include "OpenCLUtil.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace cl;

typedef std::vector<Platform>::iterator PlatformIter;

Platform getPlatform(std::string pName, cl_int &error)
{
    Platform ret_val;
    error = 0;
    try {
        // Get available platforms
        std::vector<Platform> platforms;
        Platform::get(&platforms);
        int found = -1;
        for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {
            std::string temp = it->getInfo<CL_PLATFORM_NAME>();
            std::cout<<" Current platform : "<<temp<<std::endl;
            if (temp.find(pName)!=std::string::npos) {
                found = it - platforms.begin();
                std::cout<<"Found platform: "<<temp<<std::endl;
                break;
            }
        }
        if (found==-1) {
            // Going towards + numbers to avoid conflict with OpenCl error codes
            error = +1; // requested platform not found
        } else {
            ret_val = platforms[found];
        }
    } catch(Error err) {
        std::cout << err.what() << "(" << err.err() << ")" << std::endl;
        error = err.err();
    }
    return ret_val;
}


Platform getPlatform()
{
    cl_int errCode;
    Platform plat = getPlatform(NVIDIA_PLATFORM, errCode);
    if (errCode != CL_SUCCESS) {
        Platform plat = getPlatform(AMD_PLATFORM, errCode);
        if (errCode != CL_SUCCESS) {
            Platform plat = getPlatform(INTEL_PLATFORM, errCode);
            if (errCode != CL_SUCCESS) {
                Platform plat = getPlatform(APPLE_PLATFORM, errCode);
                if (errCode != CL_SUCCESS) {
                    exit(252);
                } else {
                    return plat;
                }
            } else
                return plat;
        } else
            return plat;
    }
    return plat;
}

bool checkExtnAvailability(Device pDevice, std::string pName)
{
    bool ret_val = true;
    try {
        // find extensions required
        std::string exts = pDevice.getInfo<CL_DEVICE_EXTENSIONS>();
        std::stringstream ss(exts);
        std::string item;
        int found = -1;
        while (std::getline(ss,item,' ')) {
            if (item==pName) {
                found=1;
                break;
            }
        }
        if (found==1) {
            std::cout<<"Found CL_GL_SHARING extension: "<<item<<std::endl;
            ret_val = true;
        } else {
            std::cout<<"CL_GL_SHARING extension not found\n";
            ret_val = false;
        }
    } catch (Error err) {
        std::cout << err.what() << "(" << err.err() << ")" << std::endl;
    }
    return ret_val;
}

Program getProgram(Context pContext, std::string file, cl_int &error)
{
    Program ret_val;
    error = 0;
    std::ifstream sourceFile(file.c_str());
    std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>()));
    try {
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
        // Make program of the source code in the context
        ret_val = Program(pContext, source);
    } catch(Error err) {
        std::cout << err.what() << "(" << err.err() << ")" << std::endl;
        error = err.err();
    }
    return ret_val;
}

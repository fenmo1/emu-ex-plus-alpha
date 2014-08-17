/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <GLES/gl.h> // for glFlush()
#include <imagine/base/GLContext.hh>
#include <imagine/logger/logger.h>
#include <imagine/base/android/sdk.hh>
#include <imagine/util/time/sys.hh>

namespace Base
{

EGLDisplay EGLContextBase::getDisplay()
{
	return eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

CallResult GLContext::init(const GLConfigAttributes &attr)
{
	auto result = EGLContextBase::init(attr);
	if(result != OK)
		return result;
	return OK;
}


void GLContext::deinit()
{
	EGLContextBase::deinit();
}

GLConfig GLContext::bufferConfig()
{
	return config;
}

void GLContext::present(Window &win)
{
	#if !defined NDEBUG
	const bool checkSwapTime = true;
	#else
	const bool checkSwapTime = false;
	#endif
	if(swapBuffersIsAsync())
	{
		// check if buffer swap blocks even though triple-buffering is used
		auto beforeSwap = checkSwapTime ? TimeSys::now() : TimeSys{};
		EGLContextBase::swapBuffers(win);
		auto afterSwap = checkSwapTime ? TimeSys::now() : TimeSys{};
		long long diffSwap = (afterSwap - beforeSwap).toNs();
		if(diffSwap > 16000000)
		{
			logWarn("buffer swap took %lldns", diffSwap);
		}
	}
	else
	{
		glFlush();
		win.presented = true;
	}
}

void AndroidGLContext::swapPresentedBuffers(Window &win)
{
	if(win.presented)
	{
		assert(!swapBuffersIsAsync()); // shouldn't set presented = true if swap is async
		win.presented = false;
		EGLContextBase::swapBuffers(win);
	}
}

bool AndroidGLContext::swapBuffersIsAsync()
{
	return Base::androidSDK() >= 16;
}

}
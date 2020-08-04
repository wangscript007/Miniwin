/* Copyright (C) 2005 The cairomm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <cairomm/ngl_surface.h>
#include <cairomm/private.h>
#include <ngl_types.h>
#include <ngl_graph.h>
#ifdef CAIRO_HAS_NGL_SURFACE
#include <cairo-ngl.h>
#endif

namespace Cairo
{


NGLSurface::NGLSurface(cairo_surface_t* cobject, bool has_reference) :
    Surface(cobject, has_reference)
{
}

static cairo_format_t cairo_format(int fmt){
    switch(fmt){
    case GPF_ARGB4444:return CAIRO_FORMAT_INVALID;
    case GPF_ARGB1555:return CAIRO_FORMAT_INVALID;
    case GPF_ABGR:  return CAIRO_FORMAT_INVALID;
    case GPF_RGB565:return CAIRO_FORMAT_RGB16_565;
    case GPF_ARGB:  return CAIRO_FORMAT_ARGB32;
    case GPF_RGB32: return CAIRO_FORMAT_RGB24;
    }
}
NGLSurface::NGLSurface(void*s,bool has_reference)
 :Surface(nullptr,has_reference){
    auto cobject=cairo_ngl_surface_create(s);
    if(has_reference)
        m_cobject = cobject;
    else
        m_cobject = cairo_surface_reference(cobject);
}

NGLSurface::~NGLSurface()
{
    // nglsurface is destroyed in base class
}

void* NGLSurface::getSurface() const
{
#ifdef CAIRO_HAS_NGL_SURFACE
    return cairo_ngl_surface_get_surface(m_cobject);
#else
    return surface;
#endif
}

RefPtr<ImageSurface> NGLSurface::get_image()
{
    RefPtr<ImageSurface> surface(new ImageSurface(cobj(), false /* no reference, owned by this win32surface*/));
    check_object_status_and_throw_exception(*this);
    return surface;
}

RefPtr<NGLSurface> NGLSurface::create(void* nglsurface,bool hasref)
{
#ifdef CAIRO_HAS_NGL_SURFACE
    auto cobject = cairo_ngl_surface_create(nglsurface);
    check_status_and_throw_exception(cairo_surface_status(cobject));
    return RefPtr<NGLSurface>(new NGLSurface(cobject, hasref/* has reference */));
#else
    return RefPtr<NGLSurface>(new NGLSurface(nglsurface));
#endif
}

RefPtr<NGLSurface> NGLSurface::create(Format format, int width, int height,bool hasref)
{
    void*nglsurface;
    nglCreateSurface(&nglsurface,width,height,0,0);
#ifdef CAIRO_HAS_NGL_SURFACE
    auto cobject=cairo_ngl_surface_create(nglsurface);
    return RefPtr<NGLSurface> (new NGLSurface(cobject,hasref));//create_with_dib(format, width, height);
#else
    return RefPtr<NGLSurface>(new NGLSurface(nglsurface));    
#endif // CAIRO_HAS_NGL_SURFACE
}


} //namespace Cairo

// vim: ts=2 sw=2 et

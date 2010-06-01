// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "thread.h"

#include <k3dsdk/imeta_object.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>
#include <k3dsdk/signal_system.h>
#include <vector>

namespace module
{

namespace qtui
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// thread

thread::thread(k3d::istreaming_bitmap_source* Source) :
	source(Source)
{
}

void thread::run()
{
	source->connect_bitmap_start_signal(sigc::mem_fun(*this, &thread::on_bitmap_start));
	source->connect_bitmap_bucket_signal(sigc::mem_fun(*this, &thread::on_bitmap_bucket));
	source->connect_bitmap_finish_signal(sigc::mem_fun(*this, &thread::on_bitmap_finish));

	k3d::imeta_object* const meta_object = dynamic_cast<k3d::imeta_object*>(source);
	return_if_fail(meta_object);
	meta_object->execute("render");
}

void thread::on_bitmap_start(k3d::istreaming_bitmap_source::coordinate Width, k3d::istreaming_bitmap_source::coordinate Height)
{
	Q_EMIT bitmap_start(Width, Height);
}

void thread::on_bitmap_bucket(k3d::istreaming_bitmap_source::coordinate XOffset, k3d::istreaming_bitmap_source::coordinate YOffset, const k3d::istreaming_bitmap_source::bucket& Bucket)
{
	Q_EMIT bitmap_bucket(XOffset, YOffset, &Bucket);
}

void thread::on_bitmap_finish()
{
	Q_EMIT bitmap_finish();
}

} // namespace aqsis

} // namespace qtui

} // namespace module


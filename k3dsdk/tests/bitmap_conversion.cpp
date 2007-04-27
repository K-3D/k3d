#include <k3dsdk/bitmap.h>

struct extract_rgb
{
	template<typename P1, typename P2>
	void operator()(const P1& src, P2& dst) const
	{
		get_color(dst, boost::gil::red_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::red_t()));
		get_color(dst, boost::gil::green_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::green_t()));
		get_color(dst, boost::gil::blue_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::blue_t()));
	}
};

struct extract_alpha
{
	template<typename P1, typename P2>
	void operator()(const P1& src, P2& dst) const
	{
		get_color(dst, boost::gil::red_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
		get_color(dst, boost::gil::green_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
		get_color(dst, boost::gil::blue_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
	}
};

namespace boost
{

namespace gil
{

typedef pixel<bits8, bgr_layout_t> bgr8_pixel_t;
typedef image<bgr8_pixel_t, false> bgr8_image_t;

} // namespace gil

} // namespace boost

int main(int argc, char* argv[])
{
	k3d::bitmap source(2, 2);
	const k3d::bitmap::view_t& source_view = view(source);
	std::fill(source_view.begin(), source_view.end(), k3d::pixel(1.0, 0.5, 0.0, 0.5));

	assert(boost::gil::get_color(source_view[0], boost::gil::red_t()) == 1.0);
	assert(boost::gil::get_color(source_view[0], boost::gil::green_t()) == 0.5);
	assert(boost::gil::get_color(source_view[0], boost::gil::blue_t()) == 0.0);
	assert(boost::gil::get_color(source_view[0], boost::gil::alpha_t()) == 0.5);

	boost::gil::bgr8_image_t destination(2, 2);
	const boost::gil::bgr8_image_t::view_t& destination_view = view(destination);

	copy_pixels(boost::gil::color_converted_view<boost::gil::bgr8_pixel_t>(source_view, extract_rgb()), destination_view);

	assert(boost::gil::get_color(destination_view[0], boost::gil::red_t()) == 255);
	assert(boost::gil::get_color(destination_view[0], boost::gil::green_t()) == 127);
	assert(boost::gil::get_color(destination_view[0], boost::gil::blue_t()) == 0);

	return 0;
}


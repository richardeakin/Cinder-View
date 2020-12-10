/*
Copyright (c) 2015-2020, Richard Eakin - All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Includes fmtlib and adds some custom formatters
https://fmt.dev/latest/api.html#udt
*/

#pragma once

#include "fmt/format.h"
#include <glm/fwd.hpp>

namespace fmt {

namespace {

auto StandardFormat( std::string &format_str, fmt::format_parse_context &ctx )
{
	auto it = std::find( ctx.begin(), ctx.end(), '}' );
	format_str = "{:" + std::string { ctx.begin(), it } + "}";
	return it;
}

} // anonymous namespace

template <>
struct formatter<glm::vec2> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::vec2 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ")",
			p.x, p.y
		);
	}
};

template <>
struct formatter<glm::vec3> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::vec3 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ", " + format_str + ")",
			p.x, p.y, p.z
		);
	}
};

template <>
struct formatter<glm::vec4> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::vec4 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ", " + format_str + ", " + format_str + ")",
			p.x, p.y, p.z, p.w
		);
	}
};

template <>
struct formatter<glm::ivec2> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::ivec2 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ")",
			p.x, p.y
		);
	}
};

template <>
struct formatter<glm::ivec3> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::ivec3 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ", " + format_str + ")",
			p.x, p.y, p.z
		);
	}
};

template <>
struct formatter<glm::ivec4> {
	std::string format_str;

	auto parse( format_parse_context &ctx)
	{ 
		return StandardFormat( format_str, ctx );
	}

	template <typename FormatContext>
	auto format( const glm::ivec4 &p, FormatContext& ctx )
	{
		return format_to(
			ctx.out(),
			"(" + format_str + ", " + format_str + ", " + format_str + ", " + format_str + ")",
			p.x, p.y, p.z, p.w
		);
	}
};

}



#pragma once

#include "common/Logger.hpp"
#include "common/StringUtils.hpp"


namespace Log
{


void Line(Logger::Level level, Logger::Type type, std::initializer_list<cs::StyledString> _line);

template <typename T>
struct Hex
{
	static_assert(std::is_integral_v<T> && !std::is_same_v<bool, T>, "T is not an Integer");

	T val;
};

template <typename T>
Hex(T) -> Hex<std::remove_cvref_t<T>>;

template <typename T>
static constexpr bool _is_hexed = false;

template <typename T>
static constexpr bool _is_hexed<Hex<T>> = true;

template <typename T>
struct Pad
{
	const T& val;
	char pad_char = '0';
	int pad_size = sizeof(uintptr_t) * 2;
};

template <typename T>
Pad(T) -> Pad<std::remove_cvref_t<T>>;

template <typename T>
static constexpr bool _is_padded = false;

template <typename T>
static constexpr bool _is_padded<Pad<T>> = true;

template <size_t N = 128>
struct _BufferedStyledString
{
	static_assert(N >= 32, "Why would you want less?");

public:

	template <typename T>
	constexpr _BufferedStyledString& format_from(const T& val)
	{
		using VT = std::decay_t<T>;

		if constexpr (_is_padded<VT>)
		{
			format_from(val.val);
			ptrdiff_t diff = val.pad_size - size_;

			if (diff <= 0)
				return *this;

			const char* from = buffer_;

			if (ptr_)
			{
				from = ptr_.get();
			}
			else
			if (!str_.empty())
			{
				from = str_.data();
				str_ = {};
			}

			size_t new_size = size_ + diff;

			if (new_size > N)
			{
				auto new_ptr = std::make_unique<char[]>(new_size);
				std::copy(from, from + size_, new_ptr.get() + diff);
				std::fill_n(new_ptr.get(), diff, val.pad_char);
				ptr_ = std::move(new_ptr);
			}
			else
			{
				std::copy_backward(from, from + size_, buffer_ + new_size);
				std::fill_n(buffer_, diff, val.pad_char);
			}

			size_ = new_size;
		}
		else
		if constexpr (std::is_same_v<std::string_view, VT>)
		{
			str_ = val;
		}
		else
		if constexpr (std::is_same_v<std::string, VT>)
		{
			format_from(std::string_view(val));
		}
		else
		if constexpr (su::is_char_ptr<VT>)
		{
			format_from(std::string_view(val));
		}
		else
		{
			if constexpr (std::is_pointer_v<VT> && !su::is_char16_ptr<VT> && !su::is_wchar_ptr<VT>)
			{
				format_from(Pad(Hex((uintptr_t)val)));
			}
			else
			if constexpr (_is_hexed<VT>)
			{
				size_ = su::u8(buffer_, sizeof(buffer_), val.val, 16);
			}
			else
			if constexpr (std::is_arithmetic_v<VT>)
			{
				size_ = su::u8(buffer_, sizeof(buffer_), val);
			}
			else
			{
				size_ = su::u8(nullptr, 0, val);
				char* ptr = buffer_;

				if (size_ > N)
				{
					ptr_ = std::make_unique<char[]>(size_);
					ptr = ptr_.get();
				}

				su::u8(ptr, size_, val);
			}
		}

		return *this;
	}

	constexpr cs::StyledString Get(cs::Color color) const
	{
		if (ptr_)
			return cs::StyledString{ { ptr_.get(), size_ }, color };

		if (!str_.empty())
			return cs::StyledString{ str_, color };

		return cs::StyledString{ { buffer_, size_ }, color };
	}

private:

	std::unique_ptr<char[]> ptr_;
	std::string_view str_;
	size_t size_ = 0;
	char buffer_[N];
};

template <typename... TArgs>
inline void Line(Logger::Level level, Logger::Type type, TArgs&&... args)
{
	constexpr size_t num_args = sizeof...(TArgs);
	_BufferedStyledString<> buffered_parts[num_args];
	cs::StyledString parts[num_args];

	[&]<size_t... I>(std::index_sequence<I...>) {
		((parts[I] = buffered_parts[I].format_from(cs::UnrapStyled(args)).Get(cs::UnrapColor(args))), ...);
	}(std::make_index_sequence<num_args>());

	Line(level, type, std::initializer_list(parts, parts + num_args));
}

template <typename... TArgs>
inline void Line(Logger::Level level, TArgs&&... args)
{
	Line(level, Logger::Type::Default, std::forward<TArgs>(args)...);
}


template <typename... TArgs>
inline void Debug(TArgs&&... args)
{
	Line(Logger::Level::Debug, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
inline void Info(TArgs&&... args)
{
	Line(Logger::Level::Info, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
inline void Warn(TArgs&&... args)
{
	Line(Logger::Level::Warning, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
inline void Error(TArgs&&... args)
{
	Line(Logger::Level::Error, std::forward<TArgs>(args)...);
}


} // namespace Log

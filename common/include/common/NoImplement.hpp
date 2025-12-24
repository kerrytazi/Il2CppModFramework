#pragma once

struct NoImplement
{
	NoImplement() = delete;
	//~NoImplement() = delete;
	NoImplement(const NoImplement&) = delete;
	NoImplement(NoImplement&&) = delete;
	NoImplement& operator=(const NoImplement&) = delete;
	NoImplement& operator=(NoImplement&&) = delete;
};

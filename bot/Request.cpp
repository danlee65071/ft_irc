#include "Request.hpp"

Request::Request(const std::string &Request)
{
	request = std::string(Request.begin(), Request.end() - 1);
	get_params_queue();
	find_prefix();
	find_command();
	find_params();
}

Request::~Request()
{}

void Request::get_params_queue()
{
	std::string::iterator it = request.begin();
	std::string::iterator space_it;

	while(it != request.end())
	{
		while(it != request.end() && *it != ' ')
			++it;
		space_it = std::find(it, request.end(), ' ');
		if (it != request.end())
		{
			params_queue.push(std::string(it, space_it));
			it = space_it;
		}
	}
}

void Request::find_prefix()
{
	if (params_queue.size() > 0 && params_queue.front()[0] == ':')
	{
		prefix = std::string(params_queue.front().begin() + 1, params_queue.front().end());
		params_queue.pop();
	}
}

void Request::find_command()
{
	if (params_queue.size() > 0)
	{
		command = params_queue.front();
		params_queue.pop();
	}
}

void Request::find_params()
{
	while (params_queue.size() > 0)
	{
		if (params_queue.front()[0] == ':')
		{
			std::string param(params_queue.front().begin() + 1, params_queue.front().end());
			params_queue.pop();
			while (params_queue.size() > 0)
			{
				param.append(" ");
				param.append(params_queue.front());
				params_queue.pop();
			}
			params.push_back(param);
		}
		else
		{
			params.push_back(params_queue.front());
			params_queue.pop();
		}
	}
}

const std::string& Request::get_prefix() const
{
	return prefix;
}

const std::string& Request::get_command() const
{
	return command;
}

const std::vector<std::string>& Request::get_params() const
{
	return params;
}

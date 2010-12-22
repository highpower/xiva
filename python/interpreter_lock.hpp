// xiva (acronym for HTTP Extended EVent Automata) is a simple HTTP server.
// Copyright (C) 2009 Yandex <highpower@yandex.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef XIVA_PYTHON_INTERPRETER_LOCK_HPP_INCLUDED
#define XIVA_PYTHON_INTERPRETER_LOCK_HPP_INCLUDED

namespace xiva { namespace python {

class interpreter_init {

public:
	interpreter_init();
	~interpreter_init();

private:
	interpreter_init(interpreter_init const &);
	interpreter_init& operator = (interpreter_init const &);

private:
	static int count_;
};

static interpreter_init init_;

class interpreter_thread_lock {

public:
	interpreter_thread_lock();
	~interpreter_thread_lock();

private:
	interpreter_thread_lock(interpreter_thread_lock const &);
	interpreter_thread_lock& operator = (interpreter_thread_lock const &);
};

class interpreter_lock {

public:
	interpreter_lock();
	~interpreter_lock();

private:
	interpreter_lock(interpreter_lock const &);
	interpreter_lock& operator = (interpreter_lock const &);
};

class interpreter_unlock {

public:
	interpreter_unlock();
	~interpreter_unlock();

private:
	interpreter_unlock(interpreter_unlock const &);
	interpreter_unlock& operator = (interpreter_unlock const &);

	void *save_;
};

class interpreter_try_lock {

public:
	interpreter_try_lock();
	~interpreter_try_lock();

	bool acquired() const;

private:
	interpreter_try_lock(interpreter_try_lock const &);
	interpreter_try_lock& operator = (interpreter_try_lock const &);

	bool acquired_;
};

inline bool
interpreter_try_lock::acquired() const {
	return acquired_;
}

}} // namespaces

#endif // XIVA_PYTHON_INTERPRETER_LOCK_HPP_INCLUDED

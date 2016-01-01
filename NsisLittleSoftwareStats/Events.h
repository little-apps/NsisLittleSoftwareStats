/*
 * Little Software Stats - NSIS Plugin
 * Copyright (C) 2008-2012 Little Apps (http://www.little-apps.org)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class EventDataValue {
public:
	enum Types {
		INT,
		STRING,
		DOUBLE,
		ULONG
	};

	EventDataValue(int value) {
		this->Type = INT;
		this->i = value;
	}

	EventDataValue(tstring value) {
		this->Type = STRING;
		this->s = value;
	}

	EventDataValue(LPCTSTR value) {
		this->Type = STRING;
		this->s = tstring(value);
	}

	EventDataValue(double value) {
		this->Type = DOUBLE;
		this->d = value;
	}

	EventDataValue(unsigned long value) {
		this->Type = ULONG;
		this->ul = value;
	}

	~EventDataValue() { }

	operator int(void) const {
		return this->i;
	}

	operator tstring(void) const {
		return this->s;
	}

	operator double(void) const {
		return this->d;
	}

	operator unsigned long(void) const {
		return this->ul;
	}

	Types Type;
	int i;
	tstring s;
	double d;
	unsigned long ul;
};

class EventData {
public:
	EventData(tstring strEventCode, tstring strSessionId, int nFlowId = 0) {
		this->Add(_T("tp"), strEventCode);
		this->Add(_T("ss"), strSessionId);
		this->Add(_T("ts"), (int)std::time(0));

		if (nFlowId != 0)
			this->Add(_T("fl"), nFlowId);
	}

	template <typename T>
	void Add(tstring name, T value) {
		EventDataValue event_value(value);

		if (this->hash_table.find(name) != this->hash_table.end())
			return;

		this->hash_table.insert(pair<tstring, EventDataValue>(name, value));
	}

	unordered_map<tstring, EventDataValue> hash_table;
};

class Events {
public:
	Events() { }
	~Events() { }

	void Add(EventData ev) {
		this->events_vector.push_back(ev);
	}


	tstring Serialize(tstring strFormat) {
		tstringstream output;
		size_t i, j;
		unordered_map<tstring, EventDataValue>::iterator it;

		if (strFormat.compare(FORMAT_JSON) == 0) {
			output << _T("[[");

			for (i = 0; i != this->events_vector.size(); i++) {
				EventData ev = this->events_vector[i];

				output << _T("{");

				j = 0;

				for (it = ev.hash_table.begin(); it != ev.hash_table.end(); ++it) {
					tstring key = it->first;
					EventDataValue value = it->second;

					output << StringFormat(_T("\"%s\": "), key.c_str());

					if (value.Type == EventDataValue::STRING) {
						tstring str = value;
						output << Enquoute(str).c_str();
					} else if (value.Type == EventDataValue::INT) {
						int n = value;
						output << StringFormat(_T("%d"), n);
					} else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						output << StringFormat(_T("%d"), static_cast<int>(d));
					} else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						output << StringFormat(_T("%u"), ul);
					}

					if (j++ < ev.hash_table.size() - 1)
						output << _T(",");
				}

				output << _T("}");

				if (i < this->events_vector.size() - 1)
					output << _T(",");
			}

			output << _T("]]");
		} else {
			output << _T("<?xml version=\"1.0\"?><data><Events>");

			for (i = 0; i != this->events_vector.size(); i++) {
				EventData ev = this->events_vector[i];

				output << _T("<Event>");

				j = 0;

				for (it = ev.hash_table.begin(); it != ev.hash_table.end(); ++it) {
					tstring key = it->first;
					EventDataValue value = it->second;

					output << StringFormat(_T("<%s>"), key.c_str());

					if (value.Type == EventDataValue::STRING) {
						tstring str = value;
						output << str;
					} else if (value.Type == EventDataValue::INT) {
						int n = value;
						output << StringFormat(_T("%d"), n);
					} else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						output << StringFormat(_T("%d"), static_cast<int>(d));
					} else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						output << StringFormat(_T("%u"), ul);
					}

					output << StringFormat(_T("</%s>"), key.c_str());
				}

				output << _T("</Event>");
			}

			output << _T("</Events></data>");
		}

		return output.str();
	}

	vector<EventData> events_vector;
};
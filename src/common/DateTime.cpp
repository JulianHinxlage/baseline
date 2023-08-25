//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "DateTime.h"
#include <time.h>

namespace baseline {

	DateTime::DateTime() {
		day = 1;
		month = 1;
		year = 1970;
		hour = 0;
		minute = 0;
		second = 0;
	}

	DateTime::DateTime(int day, int month, int year) {
		setDate(day, month, year);
		setTime(0, 0, 0);
	}

	DateTime::DateTime(int day, int month, int year, int hour, int minute, int second) {
		setDate(day, month, year);
		setTime(hour, minute, second);
	}

	void DateTime::setDate(int day, int month, int year) {
		this->day = day;
		this->day = std::max(this->day, 1);
		this->day = std::min(this->day, 31);

		this->month = month;
		this->month = std::max(this->month, 1);
		this->month = std::min(this->month, 12);

		this->year = year;
		this->year = std::max(this->year, 0);
		this->year = std::min(this->year, 10000);
	}

	void DateTime::setTime(int hour, int minute, int second) {
		this->hour = hour;
		this->hour = std::max(this->hour, 0);
		this->hour = std::min(this->hour, 23);

		this->minute = minute;
		this->minute = std::max(this->minute, 0);
		this->minute = std::min(this->minute, 59);

		this->second = second;
		this->second = std::max(this->second, 0);
		this->second = std::min(this->second, 59);
	}

	void DateTime::now(bool localTime){
		time_t t;
		time(&t);

		struct tm* tm = nullptr;
		if (localTime) {
			tm = localtime(&t);
		}
		else {
			tm = gmtime(&t);
		}

		year = tm->tm_year + 1900;
		month = tm->tm_mon + 1;
		day = tm->tm_mday;
		hour = tm->tm_hour;
		minute = tm->tm_min;
		second = tm->tm_sec;
	}

	void DateTime::resetTime() {
		hour = 0;
		minute = 0;
		second = 0;
	}

	bool DateTime::operator<(const DateTime& dateTime) const {
		if (year < dateTime.year) {
			return true;
		}
		else if (year > dateTime.year) {
			return false;
		}

		if (month < dateTime.month) {
			return true;
		}
		else if (month > dateTime.month) {
			return false;
		}

		if (day < dateTime.day) {
			return true;
		}
		else if (day > dateTime.day) {
			return false;
		}

		if (hour < dateTime.hour) {
			return true;
		}
		else if (hour > dateTime.hour) {
			return false;
		}

		if (minute < dateTime.minute) {
			return true;
		}
		else if (minute > dateTime.minute) {
			return false;
		}

		if (second < dateTime.second) {
			return true;
		}
		else if (second > dateTime.second) {
			return false;
		}

		return false;
	}

	bool DateTime::operator==(const DateTime& dateTime) const {
		return year == dateTime.year && month == dateTime.month && day == dateTime.day &&
			hour == dateTime.hour && minute == dateTime.minute && second == dateTime.second;
	}

	bool DateTime::operator!=(const DateTime& dateTime) const {
		return !operator==(dateTime);
	}

	bool DateTime::operator>(const DateTime& dateTime) const {
		return !operator<(dateTime) && operator!=(dateTime);
	}

	bool DateTime::operator<=(const DateTime& dateTime) const {
		return !operator>(dateTime);
	}

	bool DateTime::operator>=(const DateTime& dateTime) const {
		return !operator<(dateTime);
	}

	std::string DateTime::toString() const {
		return toStringDate() + " " + toStringTime();
	}

	std::string DateTime::toStringDate() const {
		std::string result;
		if (day < 10) {
			result += "0";
		}
		result += std::to_string(day);
		result += ".";
		if (month < 10) {
			result += "0";
		}
		result += std::to_string(month);
		result += ".";
		result += std::to_string(year);
		return result;
	}

	std::string DateTime::toStringTime() const {
		std::string result;
		if (hour < 10) {
			result += "0";
		}
		result += std::to_string(hour);
		result += ":";
		if (minute < 10) {
			result += "0";
		}
		result += std::to_string(minute);
		result += ":";
		if (second < 10) {
			result += "0";
		}
		result += std::to_string(second);
		return result;
	}

	bool DateTime::fromString(const std::string& str) {
		auto parts = split(str, ".");
		if (parts.size() == 3) {
			day = toInt(parts[0]);
			month = toInt(parts[1]);
			year = toInt(parts[2]);
			if (year < 100) {
				year += 2000;
			}
			setDate(day, month, year);
		}
		else {
			return false;
		}
	}

}

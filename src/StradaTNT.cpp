#include <pch.h>
/*----------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
/*----------------------------------------------------------------------------*/
#include "StradaTNT.h"
#include "tool.h"
/*----------------------------------------------------------------------------*/
#define MAX_BUFF   1024
#define CSV_READ   kip = strtok_s(NULL, ",", &next_token); if( kip == NULL ) return false; trim(kip);
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
TRNLine::TRNLine() {
    memset(name, 0, 11);
    mode = 1; way = 1; frequency = 1; speed = 400;
	flag = 0;
}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
bool TRNLine::ReadCSV(const std::string& buff) {
	nodes.clear();
	boost::tokenizer<boost::escaped_list_separator<char> > tokens(buff);
	boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
	if (it == tokens.end()) return false;
	std::string str;
	size_t st = (*it).find_first_of("\"");
	size_t ed = (*it).find_last_of("\"");
	if (st != std::string::npos && ed != std::string::npos && st + 1 < ed) str = (*it).substr(st + 1, ed - st - 1);
	else str = (*it);
	boost::trim(str);
	strcpy_s(name, 11, str.c_str());
	++it;
	if (it == tokens.end()) return false; else mode = std::stoi(*it); ++it;
	if (it == tokens.end()) return false; else way = std::stoi(*it); ++it;
	if (it == tokens.end()) return false; else frequency = std::stoi(*it); ++it;
	if (it == tokens.end()) return false; else speed = std::stoi(*it); ++it;
	if (it == tokens.end()) return false; else nNode = std::stoi(*it);

	for(int i=0; i < nNode; i++) {
		TRNNode node;
		++it; if (it == tokens.end()) return false; node.access = (*it)[0];
		++it; if (it == tokens.end()) return false;
		st = (*it).find_first_of("\"");
		ed = (*it).find_last_of("\"");
		if (st != std::string::npos && ed != std::string::npos && st + 1 < ed) str = (*it).substr(st + 1, ed - st - 1);
		else str = (*it);
		boost::trim(str);
		strcpy_s(node.name, 11, str.c_str());
		nodes.push_back(node);
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
void TRNLine::Write(FILE* fp) {
	fprintf(fp, "%-10s%2d%1d%4d%3d%3d", name, mode, way, frequency, speed, nNode);
	for(const auto& node : nodes) {
		fprintf(fp, "%c%-10s",  node.access, node.name);
	}
	fprintf(fp, "\n");
}
////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
StradaTNT::StradaTNT() {
    nLine = nMode = 0;
}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
//  -1: Fail to read 1st line
//  -2: Not TNT
//  -3: Fail to read 3rd line
//  -4: No of lines or modes is 0 or minus
//  -5: Fail to read the line name
//  -6: Fail to read line information
///////////////////////////////////////////////////////////////////////////////
void StradaTNT::Read(const char* fname)
{
	std::ifstream ifs(fname, std::ios_base::in);
	if (!ifs) {
		throw std::runtime_error("Cannot open TNT file.");
	}
	std::string buff;
	int csv;

	try {
		if (std::getline(ifs, buff).fail()) throw (1);
		std::string str = buff.substr(0, 5);
		if (str == "TNT2 ") csv = 0;
		else if (str == "TNT2*") csv = 1;
		else if (str == "TNT4*") csv = 1;
		else throw(2);
		if (std::getline(ifs, buff).fail()) throw (3);
		if (csv == 0) {
			nLine = std::stoi(buff.substr(0, 5));
			nMode = std::stoi(buff.substr(5, 5));
		}
		else if (csv == 1) {
			boost::tokenizer<boost::escaped_list_separator<char> > tokens(buff);
			boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
			if (it == tokens.end()) throw (3);
			nLine = std::stoi(*it); ++it;
			if (it == tokens.end()) throw (3);
			nMode = std::stoi(*it);
		}
		if (nLine <= 0 || nMode <= 0) throw(4);
		for (int i = 0; i < nLine; i++) {
			TRNLine line;
			if (std::getline(ifs, buff).fail()) throw (5);
			if (csv == 0) {
				str = buff.substr(0, 10); boost::trim(str);
				strcpy_s(line.name, 11, str.c_str());
				short mode = (short)std::stoi(buff.substr(10, 2));
				short way = (short)(buff[2] - '0');
				int freq = std::stoi(buff.substr(13, 4));
				int spd = std::stoi(buff.substr(17, 3));
				int num = std::stoi(buff.substr(20, 3));

				if (mode < 0)           throw  7;
				if (way < 1 || way > 2) throw  8;
				if (freq < 0)           throw  9;
				if (spd < 0)            throw 10;
				if (num < 0)            throw 11;

				line.mode = mode;
				line.way = way;
				line.frequency = freq;
				line.speed = spd;
				line.nNode = num;
				for (int j = 0; j < line.nNode; j++) {
					int idx = 23 + 11 * j;
					TRNNode node;
					node.access = buff[idx]; idx = idx + 1;
					str = buff.substr(idx, 10); boost::trim(str);
					strcpy_s(node.name, 11, str.c_str());
					line.nodes.push_back(node);
				}
			}
			else if (csv == 1) {

				if (line.ReadCSV(buff) == false) throw (-10000 - i);
			}
			lines.push_back(line);
		}

	}
	catch (int e) {
		std::string msg = "TNT2(" + std::to_string(e) + ")";
		throw std::runtime_error(msg);
	}

}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
/*
void StradaTNT::Read(const char *file_name) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, file_name, "rt");
	if(error != 0 || fp==NULL) throw std::runtime_error("Cannot open TNT2 file");
	else {
		int ret = Read(fp);
		fclose(fp);
		std::string msg;
		if (ret < 0) {
			msg = "TNT2:" + std::to_string(ret);
			throw std::runtime_error(msg);
		}
	}
}
*/
void StradaTNT::Write(char* fname) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, fname, "wt");
	if(error != 0 || fp == NULL) throw std::runtime_error("Cannot create TNT2 file");
	else {
		fprintf(fp, "TNT2 \n");
		fprintf(fp, "%5d%5d\n", nLine, nMode);
		for (int i = 0; i < nLine; i++) lines[i].Write(fp);
		fclose(fp);
	}
}
///////////////////////////////////////////////////////////////////////////////
//  
///////////////////////////////////////////////////////////////////////////////
void StradaTNT::Write(FILE* fp) {
	fprintf(fp, "TNT2 \n");
	fprintf(fp, "%5d%5d\n", nLine, nMode);
	for(int i=0; i < nLine; i++) lines[i].Write(fp);
}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
TRNLine* StradaTNT::getLine(int i) {
	return &lines[i];
}

int StradaTNT::get_route_count() {
	int c = 0;
	for(int i=0; i < nLine; i++) {
		if( lines[i].way == 2 ) c += 2;
		else c++;
	}
	return c;
}

int StradaTNT::get_node_count() {
	std::set<std::string> node_table;
	int c;
	for(const auto& line : lines) {
		for(const auto& node : line.nodes) {
			node_table.insert(node.name);
		}
	}
	c = (int)node_table.size();
	return c;
}

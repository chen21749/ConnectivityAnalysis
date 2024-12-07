#include <ogrsf_frmts.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
using namespace std;

//typedef map<int, vector<int>> PointSource;
 
vector<int> vStringSplit(const  std::string& s, const std::string& delim = ",")
{
	std::vector<int> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(atoi(s.substr(pos, len - pos).data()));
			break;
		}
		elems.push_back(atoi(s.substr(pos, find_pos - pos).data()));
		pos = find_pos + delim_len;
	}
	return elems;
}

map<int, vector<int>> readShp() {
	map<int, vector<int>> result;
	CPLSetConfigOption("SHAPE_ENCODING", "utf-8");
	GDALAllRegister();
	GDALDataset* ds;
	ds = (GDALDataset*)GDALOpenEx("../data/roadPoint.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (ds == NULL)
	{
		cout << "文件代开失败" << endl;
		return result;
	}
	OGRLayer* lay;
	lay = ds->GetLayer(0);
	if (lay == NULL)
	{
		//cout << "获取图层失败" << endl;
	}
	OGRFeatureDefn* poFDefn = lay->GetLayerDefn();
	int fieldCount = poFDefn->GetFieldCount();
	//cout << "字段遍历" << endl;
	for (int i = 0; i < fieldCount; i++)
	{
		OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(i);
		//cout << poFieldDefn->GetNameRef() << "\t" << OGRFieldDefn::GetFieldTypeName(poFieldDefn->GetType()) << endl;
	}
	lay->ResetReading();
	while (true)
	{
		OGRFeature* fea = lay->GetNextFeature();
		if (fea == NULL)
		{
			break;
		}
		//cout <<atoi(fea->GetFieldAsString("ID")) << "," << fea->GetFieldAsString("PARENT_ID") << endl;
		int nId = atoi(fea->GetFieldAsString("ID"));
		vector<int> vecParents = vStringSplit(fea->GetFieldAsString("CHILD_ID"),",");
		result[nId] = vecParents;
	}
	return result;
}
	
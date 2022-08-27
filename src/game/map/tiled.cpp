#include "tiled.h"

namespace tiled {

/// ----------------  CHANGE THIS FUNCTION TO MATCH PROPERTIES SET IN TILED ------------------ ///
Properties fillPropStruct(rapidxml::xml_node<> *propertiesNode)
{
	Properties props;

	for(auto propertyInfo = propertiesNode->first_node("property"); propertyInfo; propertyInfo = propertyInfo->next_sibling("property"))
	{
		std::string name = propertyInfo->first_attribute("name")->value();
		std::string value = propertyInfo->first_attribute("value")->value();
		if(value == "false")
			std::cout << "WARNING: false value from bool on property " << name << ", is this intentional?\n";

		//EXAMPLE PROPERTIES
		if(name == "collidable")
		{
			if(value == "true")
		    	props.collidable = true;
			else if(value == "false")
				props.collidable = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "hero")
		{
			if(value == "true")
				props.hero = true;
			else if(value == "false")
				props.hero = false;
			else
					std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "enemy")
		{
			if(value == "true")
				props.enemy = true;
			else if(value == "false")
				props.enemy = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "room")
		{
			if(value == "true")
				props.room = true;
			else if(value == "false")
				props.room = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "obstacle")
		{
			if(value == "true")
				props.obstacle = true;
			else if(value == "false")
				props.obstacle = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "checkpoint")
		{
			if(value == "true")
				props.checkpoint = true;
			else if(value == "false")
				props.checkpoint = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "pickup")
		{
			if(value == "true")
				props.pickup = true;
			else if(value == "false")
				props.pickup = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "stone")
		{
		    props.stone = std::stoi(value);
		}
		else if(name == "gold")
		{
			if(value == "true")
				props.gold = true;
			else if(value == "false")
				props.gold = false;
			else
				std::cout << "WARNING: property " << name << " did not have true or false value!" << std::endl;
		}
		else if(name == "smoke")
		{
			 props.smoke = std::stoi(value);
		}
		else
		{
			std::cout << "WARNING: property " << name << " not recognised!\n";
		}
	}

	return props;
}

  
Tileset::Tileset(std::string filename)
{
	if(filename.length() < 4)
		throw std::runtime_error("failed to load text file at " + filename + " \ntilemap filename is invalid");
	else if(filename.substr(filename.length() - 3, 3) != "tsx")
		throw std::runtime_error("failed to load text file at " + filename + " \ntilemap filename is not .tsx");

	char* tilesetText = loadTextFile(filename);
	if(tilesetText == nullptr)
		throw std::runtime_error("failed to load text file at " + filename);

	rapidxml::xml_document<> tilesetXML;
	tilesetXML.parse<0>(tilesetText);

	auto tilesetInfo = tilesetXML.first_node("tileset");
	if(tilesetInfo == nullptr)
		throw std::runtime_error("tileset at " + filename + " has no tileset node");

	this->name = tilesetInfo->first_attribute("name")->value();
	this->tileWidth = std::atoi(tilesetInfo->first_attribute("tilewidth")->value());
	this->tileHeight = std::atoi(tilesetInfo->first_attribute("tileheight")->value());
	this->tileCount = std::atoi(tilesetInfo->first_attribute("tilecount")->value());
	this->columns = std::atoi(tilesetInfo->first_attribute("columns")->value());

	if(tilesetInfo->first_attribute("spacing"))
		this->spacing  = std::atoi(tilesetInfo->first_attribute("spacing")->value());
	if(tilesetInfo->first_attribute("margin"))
		this->margin  = std::atoi(tilesetInfo->first_attribute("margin")->value());

	auto imageInfo = tilesetInfo->first_node("image");
	if(imageInfo == nullptr)
		throw std::runtime_error("tileset at " + filename + " has no image node");

	this->imageSource = imageInfo->first_attribute("source")->value();
	this->imageWidth = std::atoi(imageInfo->first_attribute("width")->value());
	this->imageHeight = std::atoi(imageInfo->first_attribute("height")->value());

	size_t lastpos = imageSource.find_last_of('/');
	if(lastpos == std::string::npos)
		lastpos = 0;
	this->imageSource = TILED_TEXTURE_LOCATION + imageSource.substr(lastpos);

	delete tilesetText;
}


Map::Map(std::string filename)
{
	if(filename.length() < 4)
		throw std::runtime_error("failed to load text file at " + filename + " \nmap filename is invalid");
	else if(filename.substr(filename.length() - 3, 3) != "tmx")
		throw std::runtime_error("failed to load text file at " + filename + " \nmap filename is not .tmx");
	char* MapText = loadTextFile(filename);

	if(MapText == nullptr)
		throw std::runtime_error("failed to load text file at " + filename);

	rapidxml::xml_document<> mapXML;
	mapXML.parse<0>(MapText);
	auto mapInfo = mapXML.first_node("map");
	if(mapInfo == nullptr)
		throw std::runtime_error("map at " + filename + " has no map node");

	this->width = std::atoi(mapInfo->first_attribute("width")->value());
	this->height = std::atoi(mapInfo->first_attribute("height")->value());
	this->tileWidth = std::atoi(mapInfo->first_attribute("tilewidth")->value());
	this->tileHeight = std::atoi(mapInfo->first_attribute("tileheight")->value());

	totalTiles = 0;

	for(auto tilesetInfo = mapInfo->first_node("tileset"); tilesetInfo; tilesetInfo = tilesetInfo->next_sibling("tileset"))
	{
		std::string tilesetInfoLoc = tilesetInfo->first_attribute("source")->value();
		size_t lastpos = tilesetInfoLoc.find_last_of('/');
		if(lastpos == std::string::npos)
			lastpos = 0;
		tilesets.push_back(Tileset(TILEMAP_LOCATION + tilesetInfoLoc.substr(lastpos)));

		tilesets.back().firstTileID = std::atoi(tilesetInfo->first_attribute("firstgid")->value());
		if(tilesets.back().tileWidth != this->tileWidth || tilesets.back().tileHeight != this->tileHeight)
			throw std::runtime_error("tileset tile dimentions do not match map tile dimentions");
		totalTiles += tilesets.back().tileCount;
	}

	for(auto layerInfo = mapInfo->first_node("layer"); layerInfo; layerInfo = layerInfo->next_sibling("layer"))
	{
		layers.push_back(Layer());

		auto propertiesNode = layerInfo->first_node("properties");
		if(propertiesNode != nullptr)
			layers.back().props = fillPropStruct(propertiesNode);
		else
			layers.back().props = Properties();
		auto dataNode = layerInfo->first_node("data");
		if(dataNode == nullptr)
			throw std::runtime_error("layer of map at " + filename + " has no data node");

		std::string currentNum = "";
		for(size_t i = 0; i < dataNode->value_size(); i++)
		{
			switch(dataNode->value()[i])
			{
				case '\n':
					break;
				case ',':
					if(currentNum.size() == 0)
						throw std::runtime_error("layer of map at " + filename + " has blank tile data");
					layers.back().data.push_back(std::stoi(currentNum));
					currentNum = "";
					break;
				default:
					currentNum += dataNode->value()[i];
					break;
			}
		}
		if(currentNum.size() == 0)
			throw std::runtime_error("layer of map at " + filename + " has blank tile data");
		layers.back().data.push_back(std::stoi(currentNum));
		if(layers.back().data.size() != this->width * this->height)
			throw std::runtime_error("layer of map at " + filename + " has different dimentions than map");
	}

	for(auto objGroupInfo = mapInfo->first_node("objectgroup"); objGroupInfo; objGroupInfo = objGroupInfo->next_sibling("objectgroup"))
	{
		objectGroups.push_back(ObjectGroup());

		auto propertiesNode = objGroupInfo->first_node("properties");
		if(propertiesNode != nullptr)
			objectGroups.back().props = fillPropStruct(propertiesNode);
		else
			objectGroups.back().props = Properties();

		for(auto objectInfo = objGroupInfo->first_node("object"); objectInfo; objectInfo = objectInfo->next_sibling("object"))
		{
			auto objTextNode =  objectInfo->first_node("text");
			auto objPolygonNode = objectInfo->first_node("polygon");
			auto objPolylineNode = objectInfo->first_node("polyline");
			Object* fillObj = nullptr;
			if(objTextNode != nullptr)
			{
				texts.push_back(Text());
				if(objTextNode->first_attribute("pixelsize") != nullptr)
					texts.back().pixelSize = std::atoi(objTextNode->first_attribute("pixelsize")->value());
				texts.back().text = objTextNode->value();

				std::string colour = objTextNode->first_attribute("color") != nullptr ? objTextNode->first_attribute("color")->value() : "";

				if(colour != "")
				{
					texts.back().colour.r =  std::stoi(colour.substr(1, 2), nullptr, 16);
					texts.back().colour.g =  std::stoi(colour.substr(3, 2), nullptr, 16);
					texts.back().colour.b =  std::stoi(colour.substr(5, 2), nullptr, 16);
					texts.back().colour.a =  colour.size() > 7 ? std::stoi(colour.substr(7, 2), nullptr, 16) : 255;
				}
				else
				{
					texts.back().colour = Colour(0, 0, 0, 255);
				}
				fillObj = &texts.back().obj;
			}
			else if(objPolygonNode != nullptr || objPolylineNode !=  nullptr)
			{
			 	objectGroups.back().polys.push_back(PolygonObject());
				if(objPolylineNode != nullptr)
				{
					objectGroups.back().polys.back().closed = false;
					objPolygonNode = objPolylineNode;
				}
				std::string points = objPolygonNode->first_attribute("points")->value();
				int startIndex = 0;
				int p1 = 0;
				for(int i = 0; i < points.size();  i++)
				{
					if(points[i] == ',')
					{
						p1 = std::stoi(points.substr(startIndex, i - startIndex));
						startIndex = i+1;
					}
					if(points[i] == ' ')
					{
						objectGroups.back().polys.back().points.push_back(
							Point(
								(double)p1,
								(double)std::stoi(points.substr(startIndex, i - startIndex))
							)
						);
						startIndex = i+1;
					}
				}
				objectGroups.back().polys.back().points.push_back(
					Point(
						(double)p1,
						(double)std::stoi(points.substr(startIndex, points.size() - startIndex))
					)
				);

				fillObj = &objectGroups.back().polys.back().obj;
			}
			else
			{
				objectGroups.back().objs.push_back(Object());
				fillObj = &objectGroups.back().objs.back();
			}

			auto objPropertiesNode = objectInfo->first_node("properties");
			if(objPropertiesNode != nullptr)
				fillObj->props = fillPropStruct(objPropertiesNode);
			else
				fillObj->props = Properties();

			auto x = objectInfo->first_attribute("x");
			auto y = objectInfo->first_attribute("y");
			if(x != nullptr && y != nullptr)
			{
				fillObj->x = std::atof(x->value());
				fillObj->y = std::atof(y->value());
			}
			else
				std::cout << "WARNING: object without coords" << std::endl;

			auto w = objectInfo->first_attribute("width");
			if(w != nullptr)
				fillObj->w = std::atof(w->value());
			auto h = objectInfo->first_attribute("height");
			if(h != nullptr)
				fillObj->h = std::atof(h->value());

		}
	}


	for(auto imgLayerInfo = mapInfo->first_node("imagelayer"); imgLayerInfo; imgLayerInfo = imgLayerInfo->next_sibling("imagelayer"))
	{
		imgLayer.push_back(ImageLayer());
		if(imgLayerInfo->first_attribute("offsetx") != nullptr)
		{
		imgLayer.back().x = std::atof(imgLayerInfo->first_attribute("offsetx")->value());
		imgLayer.back().y = std::atof(imgLayerInfo->first_attribute("offsety")->value());
		}
		std::string imageSource = imgLayerInfo->first_node("image")->first_attribute("source")->value();
		size_t lastpos = imageSource.find_last_of('/');
		if(lastpos == std::string::npos)
			lastpos = 0;
		imgLayer.back().source = TILED_IMAGE_LOCATION + imageSource.substr(lastpos);
	}

	delete MapText;
}

char* loadTextFile(std::string filename)
{
	std::ifstream in(filename, std::ios::binary | std::ios::ate);
	if (!in.is_open())
		throw std::runtime_error("failed to load text file at " + filename);

	size_t fileSize = (size_t)in.tellg();
	char* text = new char[fileSize + 1]; //+1 for null terminator

	in.seekg(0);
	in.read(text, fileSize);
	in.close();
	text[fileSize] = '\0';
	return text;
}

}

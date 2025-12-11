#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector> //this is a list basically


void clean_string(std::string &cleanee, const std::string &cleaner) {
  // cleanee -> name="GA1194500inner"
  // cleaner -> name="GA
  
  size_t start_pos = cleanee.find(cleaner);
  if (start_pos == std::string::npos) {
    cleanee.clear();
  }

  size_t end_position = start_pos + cleaner.length();
  if (end_position == std::string::npos) {
    cleanee.clear();
  }

  std::string clean_string = cleanee.erase(start_pos, end_position-1);
}


std::vector<std::string> split(const std::string& s, char seperator) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(s);

  while (std::getline(token_stream, token, seperator)) {
    tokens.push_back(token);
  }

  return tokens;
}


std::string get_max_damage(const std::string& xml_data, const std::string location){
  // the function gets a location and returns the maximum damage for that position

  std::string start_tag = "<fx_gpos " + location + ">";
  std::string end_tag = "</fx_pos>";  
  std::string damage_tag = "<fx_damage_wire_pos";
  std::string damage_str;
  size_t pointer_position;

  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }

  pointer_position = start_position;

  while (true) {
    size_t damage_pos = xml_data.find(damage_tag, pointer_position);
    if (damage_pos == std::string::npos) {
      break;
    }
    damage_pos += damage_tag.length(); // advancing the pointer to the end of the tag
    // the size of the damage word is hard coded
    size_t chunk = 32 * sizeof(char);
    damage_str = xml_data.substr(damage_pos, chunk);
    clean_string(damage_str, " damage=\"");
    clean_string(damage_str,"\"");

    pointer_position += damage_pos;
    // std::cout << damage_str << std::endl;
  }

  return damage_str;
}


std::string get_value(const std::string &xml_data, const std::string &tag) {
  // function to extract the value of a given field from tag
  std::string start_tag = "<" + tag + ">";
  std::string end_tag = "</" + tag + ">";

  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }

  start_position += start_tag.length();

  size_t end_position = xml_data.find(end_tag, start_position);
  if (end_position == std::string::npos) {
    return "";
  }

  return xml_data.substr(start_position, end_position - start_position);
}


std::string get_attribute(const std::string &xml_data, const std::string &tag) {
  // opening and closing tags
  std::string start_tag = "<" + tag;
  // std::string end_tag = "</" + tag + ">";

  // find the tag starting
  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }
  start_position += start_tag.length();

  // find the next closing bracket of the start tag
  size_t closing_position = xml_data.find(">", start_position);
  if (closing_position == std::string::npos) {
    return "";
  }

  return xml_data.substr(start_position, closing_position - start_position);
}


std::string get_all_gpos(const std::string &xml_data, const std::string &tag) {
  // initiating position
  size_t reader_position = 0;
  std::string gpos;
  std::string single_gpos;

  std::string start_tag = "<" + tag;
  std::string end_tag = "</" + tag + ">";

  while (true) {
    size_t start_position = xml_data.find(start_tag, reader_position);
    if (start_position == std::string::npos) {
      break;
    };
    start_position += start_tag.length(); // moving the postion to contents

    size_t closing_postion = xml_data.find(">", start_position);
    if (closing_postion == std::string::npos) {
      break;
    };

    single_gpos = xml_data.substr(start_position, closing_postion - start_position);
    // clean_string(single_gpos, "name=\"GA");
    // clean_string(single_gpos, "inner");
    gpos += single_gpos;

    size_t end_position = xml_data.find(end_tag, closing_postion);
    if( end_position == std::string::npos) {
      break;
    };

    reader_position = end_position + end_tag.length();
  };
  
  return gpos;
}


int main() {
  // open document
  std::ifstream file("./G07_TDP_intact_NOV_SN_Corrosive_inner_worst.xml");

  // handle error
  if (!file.is_open()) {
    std::cout << "Error opening the file" << std::endl;
    return 1;
  }

  // read the entire file
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();
  file.close();

  std::cout << "----------- Processing the XML file ----------" << std::endl;

  // test the processing

  std::string damage_loadcase = get_value(file_content, "fx_damage_loadcase");

  // print the data extract
  std::cout << "Printing the content of a tag" << std::endl;
  std::cout << damage_loadcase << std::endl;

  // print the atributes of a tag
  std::cout << "Printing the attributes of a tag" << std::endl;
  std::cout << get_attribute(file_content, "fx_gpos") << std::endl;

  // print all positions as test
  std::cout << "------- printing the gpos data --------" << std::endl;
  std::string gpos_2 = get_all_gpos(file_content, "fx_gpos");
  std::cout << gpos_2 << std::endl;

  std::vector<std::string> gpos_list = split(gpos_2, ' ');
  for (const auto& gpos : gpos_list) {
    std::cout << gpos << std::endl;
    // get_max_damage(file_content, gpos);
    // break;
  }
  std::string string_test = get_max_damage(file_content, gpos_list[1]);
  std::cout << string_test << std::endl;

  return 0;
}

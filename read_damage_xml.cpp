#include<iostream>
#include<string>
#include<fstream>
#include<sstream>


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

int main() {

    // open document
    std::ifstream file("./G07_TDP_intact_NOV_SN_Corrosive_inner_worst.xml");

    //handle error
    if (!file.is_open()) {
        std::cout << "Error opening the file" << std::endl;
        return 1;
    }

    // read the entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_content = buffer.str();
    file.close();

    std::cout << "PRocessing the XML file" << std::endl;

    // test the processing
    //

    std::string damage_loadcase = get_value(file_content, "fx_damage_loadcase");

    std::cout << damage_loadcase << std::endl;
    


    return 0;
}

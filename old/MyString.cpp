//#include<string>
//
//namespace HW{
//	string UpcaseString(const string& string){
//		const char* str = string.c_str();
//		int length = string.length();
//		char dif = 'a' - 'A';
//		char*newStr = new char[length + 1];
//		//memcpy(newStr, string.c_str(), length + 1);
//		for (int i = 0; i < length; i++){
//			if (newStr[i] >= 'a' && newStr[i] <= 'z')
//				newStr[i] -= dif;
//		}
//		string resStr = newStr;
//		delete[] newStr;
//		return resStr;
//	}
//}
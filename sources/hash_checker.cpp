// Copyright 2020 Your Name <your_email>

#include <hash_checker.hpp>

bool check_hash(int n, const std::string& hash){
  auto size = hash.length();
  for(auto i = size-1; i >= size - n; i--){
    if (hash[i] != '0'){
      return false;
    }
  }
  return true;
}
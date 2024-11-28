#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class ChaoticStreamCipher {
public:
  ChaoticStreamCipher(double r = 0.3, double c0 = 0.4)
      : r(r), c(c0), initialized(false) {
    assert(c0 <= 1);
    assert(c0 >= 0);
  }

  void initializeKeyStream(size_t length) {
    if (initialized)
      return; // 防止重复初始化
    key_stream.resize(length);
    for (size_t i = 0; i < length; ++i) {
      key_stream[i] = static_cast<int>(logisticMap(c) * 255) + 1;
      c = logisticMap(c); // 生成下一个混沌值
    }

    initialized = true;
  }

  std::string encrypt(const std::string &plaintext) {
    size_t len = plaintext.size();
    if (!initialized || key_stream.size() < len) {
      initializeKeyStream(len); // 如果需要，则初始化或扩展密钥流
    }

    std::string ciphertext;
    ciphertext.reserve(len);

    for (size_t i = 0; i < len; ++i) {
      // 加密每个字符，使用XOR与混沌密钥
      char encrypted_char = plaintext[i] ^ key_stream[i];
      if (encrypted_char == '\0') {
        ciphertext.push_back(plaintext[i]);
      } else
        ciphertext.push_back(encrypted_char);
    }

    return ciphertext;
  }

  std::string decrypt(const std::string &ciphertext) {
    size_t len = ciphertext.size();
    if (!initialized || key_stream.size() < len) {
      // 在这种情况下，理论上应该抛出一个错误，因为密钥流不匹配
      // 但为了简单起见，我们在这里重新初始化（这在实际应用中是不安全的）
      // 正确的做法是在加密后保存密钥流，并在解密时加载它
      initializeKeyStream(len); // 这仅用于演示目的，不应该在生产代码中使用
    }

    std::string decryptedtext;
    decryptedtext.reserve(len);

    for (size_t i = 0; i < len; ++i) {
      // 解密每个字符，使用与加密相同的XOR操作
      char decrypted_char = ciphertext[i] ^ key_stream[i];
      if (decrypted_char == '\0') {
        decryptedtext.push_back(ciphertext[i]);
      } else
        decryptedtext.push_back(decrypted_char);
    }

    return decryptedtext;
  }

private:
  double logisticMap(double c) {
    if (c <= r && c >= 0) {
      return c / r;
    } else if (c > r && c <= 0.5) {
      return (c - r) / (0.5 - r);
    } else if (c >= 0 && c <= 1) {
      return logisticMap(1 - c);
    } else {
      throw std::runtime_error("c must >=0 and <= 1!");
    }
  }

  double r; // Logistic map 参数
  double c; // 初始条件
  double T1;
  std::vector<int> key_stream; // 存储整数密钥流
  bool initialized;            // 标记密钥流是否已初始化
};
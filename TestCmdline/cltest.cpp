/*
  以下のことを行うサンプルコード
  - cmdline.h を使って，コマンドライン引数をパースする
    使い方は以下を参照
    https://tanakh.hatenablog.com/entry/20091028/p1
    https://github.com/tanakh/cmdline
  - "input_file" を読み込んで "output_file" に出力．
    バイトサイズを "output_file".bin にバイナリ形式で出力．
*/
#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include "cmdline.h"

using namespace std;

int main(int argc, char *argv[])
{
  cmdline::parser p; // parser を定義
  p.add<string>("input_file",  'i', "input file name",  true);
  p.add<string>("output_file", 'o', "output file name", true);

  p.parse_check(argc, argv);
  const string in_filename = p.get<string>("input_file");
  const string out_filename = p.get<string>("output_file");

  auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
  std::ifstream in_stream(in_filename);

  cout << "in_filename = " << in_filename << endl;
  cout << "out_filename = " << out_filename << endl;
  // 入力ファイルのバイト数を計算
  const uint64_t bytesize = in_stream.seekg(0, std::ios::end).tellg();
  in_stream.clear(); // フラグを元に戻す
  in_stream.seekg(0, std::ios::beg); // 読み込み位置を先頭に戻す
  cout << "bytesize = " << bytesize << endl;

  const string out_file = p.get<string>("output_file");

  string out_file_byte = string(out_file).append(".bin");
  {
    std::ofstream out_stream(out_file_byte);
    out_stream.write((char*) & bytesize, sizeof(bytesize)); // bytesize をバイナリ形式で書き込む
  }
  {
    std::ofstream out_stream(out_file);
    for (uint64_t i = 0; i < bytesize; ++i) {
      char c;
      in_stream.read((char*) & c, sizeof(c)); // 文字を読み込む
      out_stream.write((char*) & c, sizeof(c)); // 文字を書き込む
    }
  }

  auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
  double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
  std::cout << millisec << " ms" << std::endl;

  return 1;
}


/*
 * Copyright 2023 Chris Benesch
 *
 * fcgi_request_cpp - A somewhat simple post processor for FastCGI
 * requests to put in front of your CGI/C++ based application. It's
 * a common thing to have to reinvent, and this saves that time
 *
 * Compare and inspired by the ancient ccgi package from GNU
 *
 * MIT Standard distribution license
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <fcgi_request_cpp.hxx>

namespace FCGI {

FCGIData base64Decode(std::string &in)
{
  std::string::size_type i;
  char dtable[256];

  for(i= 0;i<255;i++){
    dtable[i]= 0x80;
  }
  for(i= 'A';i<='I';i++){
    dtable[i]= 0+(i-'A');
  }
  for(i= 'J';i<='R';i++){
    dtable[i]= 9+(i-'J');
  }
  for(i= 'S';i<='Z';i++){
    dtable[i]= 18+(i-'S');
  }
  for(i= 'a';i<='i';i++){
    dtable[i]= 26+(i-'a');
  }
  for(i= 'j';i<='r';i++){
    dtable[i]= 35+(i-'j');
  }
  for(i= 's';i<='z';i++){
    dtable[i]= 44+(i-'s');
  }
  for(i= '0';i<='9';i++){
    dtable[i]= 52+(i-'0');
  }
  dtable['+']= 62;
  dtable['/']= 63;
  dtable['=']= 0;
  std::string::size_type idx = 0;
  const std::string::size_type inLen = in.length();
  FCGIData out;
  while(idx < inLen){
    char a[4],b[4],o[3];

    char inbuf[4] = {0,};
    for(i = 0; i<4; i++) {
      char c = inbuf[i] = in.at(i+idx);

      if(i+idx > inLen){
        return FCGIData();
      }

      if(dtable[c]&0x80){
        fprintf(stderr,"Illegal character '%c' in base64 data.\n",c);
        return FCGIData();
      }

      a[i]= c;
      b[i]= dtable[c];
    }
    o[0]= (b[0]<<2)|(b[1]>>4);
    o[1]= (b[1]<<4)|(b[2]>>2);
    o[2]= (b[2]<<6)|b[3];
    i= a[2]=='='?1:(a[3]=='='?2:3);
    for (int j = 0; j < i; j++)
      out.append(o[j]);
    idx += i;
  }
  return out;
}


std::string base64Encode(FCGIData &dat)
{
  char dtable[256] = { 0, };

  for(int i= 0;i<9;i++){
    dtable[i]= 'A'+i;
    dtable[i+9]= 'J'+i;
    dtable[26+i]= 'a'+i;
    dtable[26+i+9]= 'j'+i;
  }
  for(int i= 0;i<8;i++){
    dtable[i+18]= 'S'+i;
    dtable[26+i+18]= 's'+i;
  }
  for(int i= 0;i<10;i++){
    dtable[52+i]= '0'+i;
  }
  dtable[62]= '+';
  dtable[63]= '/';

  size_t idx = 0;
  const size_t datSz = dat.size();

  std::string rv;
  while(idx <= datSz)
  {
    char igroup[3],ogroup[4];
    int c,n;

    igroup[0]= igroup[1]= igroup[2]= 0;
    for(n = 0;n<3;n++)
    {
      c = dat.at(idx+n);
      if(c==0xff){
        break;
      }
      igroup[n]= c;
    }
    if(n > 0){
      ogroup[0]= dtable[igroup[0]>>2];
      ogroup[1]= dtable[((igroup[0]&3)<<4)|(igroup[1]>>4)];
      ogroup[2]= dtable[((igroup[1]&0xF)<<2)|(igroup[2]>>6)];
      ogroup[3]= dtable[igroup[2]&0x3F];
      if(n<3){
        ogroup[3]= '=';
        if(n<2){
          ogroup[2]= '=';
        }
      }
      for(int i= 0;i<4;i++){
        rv.append({ogroup[i]});
      }
    }
  }
  return rv;
}

}

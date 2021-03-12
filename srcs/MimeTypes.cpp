# include "MimeTypes.hpp"

std::map<std::string, std::string, ft::comp> MimeTypes::mimes_;

void MimeTypes::initMap() {
  mimes_[".html"] = "text/html";
  mimes_[".htm"] = "text/html";
  mimes_[".shtml"] = "text/html";
  mimes_[".css"] = "text/css";
  mimes_[".xml"] = "text/xml";
  mimes_[".gif"] = "image/gif";
  mimes_[".jpeg"] = "image/jpeg";
  mimes_[".jpg"] = "image/jpeg";
  mimes_[".js"] = "application/javascript";
  mimes_[".atom"] = "application/atom+xml";
  mimes_[".rss"] = "application/rss+xml";

  mimes_[".mml"] = "text/mathml";
  mimes_[".txt"] = "text/plain";
  mimes_[".jad"] = "text/vnd.sun.j2me.app-descriptor";
  mimes_[".wml"] = "text/vnd.wap.wml";
  mimes_[".htc"] = "text/x-component";

  mimes_[".png"] = "image/png";
  mimes_[".svg"] = "image/svg+xml";
  mimes_[".svgz"] = "image/svg+xml";
  mimes_[".tif"] = "image/tiff";
  mimes_[".tiff"] = "image/tiff";
  mimes_[".wbmp"] = "image/vnd.wap.wbmp";
  mimes_[".webp"] = "image/webp";
  mimes_[".ico"] = "image/x-icon";
  mimes_[".jng"] = "image/x-jng";
  mimes_[".bmp"] = "image/x-ms-bmp";

  mimes_[".woff"] = "font/woff";
  mimes_[".woff2"] = "font/woff2";

  mimes_[".jar"] = "application/java-archive";
  mimes_[".war"] = "application/java-archive";
  mimes_[".ear"] = "application/java-archive";
  mimes_[".json"] = "application/json";
  mimes_[".hqx"] = "application/mac-binhex40";
  mimes_[".doc"] = "application/msword";
  mimes_[".pdf"] = "application/pdf";
  mimes_[".ps"] = "application/postscript";
  mimes_[".eps"] = "application/postscript";
  mimes_[".ai"] = "application/postscript";
  mimes_[".rtf"] = "application/rtf";
  mimes_[".m3u8"] = "application/vnd.apple.mpegurl";
  mimes_[".kml"] = "application/vnd.google-earth.kml+xml";
  mimes_[".kmz"] = "application/vnd.google-earth.kmz";
  mimes_[".xls"] = "application/vnd.ms-excel";
  mimes_[".eot"] = "application/vnd.ms-fontobject";
  mimes_[".ppt"] = "application/vnd.ms-powerpoint";
  mimes_[".odg"] = "application/vnd.oasis.opendocument.graphics";
  mimes_[".odp"] = "application/vnd.oasis.opendocument.presentation";
  mimes_[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
  mimes_[".odt"] = "application/vnd.oasis.opendocument.text";

  mimes_[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
  mimes_[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  mimes_[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

  mimes_[".wmlc"] = "application/vnd.wap.wmlc";
  mimes_[".7z"] = "application/x-7z-compressed";
  mimes_[".cco"] = "application/x-cocoa";
  mimes_[".jardiff"] = "application/x-java-archive-diff";
  mimes_[".jnlp"] = "application/x-java-jnlp-file";
  mimes_[".run"] = "application/x-makeself";
  mimes_[".pl"] = "application/x-perl";
  mimes_[".pm"] = "application/x-perl";
  mimes_[".prc"] = "application/x-pilot";
  mimes_[".pdb"] = "application/x-pilot";
  mimes_[".rar"] = "application/x-rar-compressed";
  mimes_[".rpm"] = "application/x-redhat-package-manager";
  mimes_[".sea"] = "application/x-sea";
  mimes_[".swf"] = "application/x-shockwave-flash";
  mimes_[".sit"] = "application/x-stuffit";
  mimes_[".tcl"] = "application/x-tcl";
  mimes_[".tk"] = "application/x-tcl";
  mimes_[".der"] = "application/x-x509-ca-cert";
  mimes_[".pem"] = "application/x-x509-ca-cert";
  mimes_[".crt"] = "application/x-x509-ca-cert";
  mimes_[".xpi"] = "application/x-xpinstall";
  mimes_[".xhtml"] = "application/xhtml+xml";
  mimes_[".xspf"] = "application/xspf+xml";
  mimes_[".zip"] = "application/zip";

  mimes_[".bin"] = "application/octet-stream";
  mimes_[".exe"] = "application/octet-stream";
  mimes_[".dll"] = "application/octet-stream";
  mimes_[".deb"] = "application/octet-stream";
  mimes_[".dmg"] = "application/octet-stream";
  mimes_[".iso"] = "application/octet-stream";
  mimes_[".img"] = "application/octet-stream";
  mimes_[".msi"] = "application/octet-stream";
  mimes_[".msp"] = "application/octet-stream";
  mimes_[".msm"] = "application/octet-stream";

  mimes_[".mid"] = "audio/midi";
  mimes_[".midi"] = "audio/midi";
  mimes_[".kar"] = "audio/midi";
  mimes_[".mp3"] = "audio/mpeg";
  mimes_[".ogg"] = "audio/ogg";
  mimes_[".m4a"] = "audio/x-m4a";
  mimes_[".ra"] = "audio/x-realaudio";

  mimes_[".3gpp"] = "video/3gpp";
  mimes_[".3gp"] = "video/3gpp";
  mimes_[".ts"] = "video/mp2t";
  mimes_[".mp4"] = "video/mp4";
  mimes_[".mpeg"] = "video/mpeg";
  mimes_[".mpg"] = "video/mpeg";
  mimes_[".mov"] = "video/quicktime";
  mimes_[".webm"] = "video/webm";
  mimes_[".flv"] = "video/x-flv";
  mimes_[".m4v"] = "video/x-m4v";
  mimes_[".mng"] = "video/x-mng";
  mimes_[".asx"] = "video/x-ms-asf";
  mimes_[".asf"] = "video/x-ms-asf";
  mimes_[".wmv"] = "video/x-ms-wmv";
  mimes_[".avi"] = "video/x-msvideo";
}

std::string MimeTypes::getType(std::string extension) {
  initMap();
  if (!extension.length())
    return "application/octet-stream";
  if (mimes_.count(extension))
    return mimes_[extension];
  return "application/octet-stream";
};
--Created 5.7.2014 as part of SSM SafeWriting project, this file contains Lua utilities using UTF-8 which can be useful sometimes
UTF8Clean={
--Cyrillic:
	['Я']= 'Ya',
	['Ё']= 'Yo',
	['Ю']= 'Yu',
	['Щ']= 'Shch',
	['Ш']= 'Sh',
	['Ж']= 'Zh',
	['Ч']= 'Ch',
	['А']= 'A',
	['Б']= 'B',
	['Ц']= 'C',
	['Д']= 'D',
	['Е']= 'E',
	['Ф']= 'F',
	['Г']= 'G',
	['Х']= 'H',
	['И']= 'I',
	['Й']= 'J',
	['К']= 'K',
	['Л']= 'L',
	['М']= 'M',
	['Н']= 'N',
	['О']= 'O',
	['П']= 'P',
	['Р']= 'R',
	['С']= 'S',
	['Т']= 'T',
	['У']= 'U',
	['В']= 'V',
	['Ы']= 'Y',
	['З']= 'Z',
	['я']= 'ya',
	['ё']= 'yo',
	['ю']= 'yu',
	['щ']= 'shch',
	['ш']= 'sh',
	['ж']= 'zh',
	['ч']= 'ch',
	['а']= 'a',
	['б']= 'b',
	['ц']= 'c',
	['д']= 'd',
	['е']= 'e',
	['ф']= 'f',
	['г']= 'g',
	['х']= 'h',
	['и']= 'i',
	['й']= 'j',
	['к']= 'k',
	['л']= 'l',
	['м']= 'm',
	['н']= 'n',
	['о']= 'o',
	['п']= 'p',
	['р']= 'r',
	['с']= 's',
	['т']= 't',
	['у']= 'u',
	['в']= 'v',
	['ы']= 'y',
	['з']= 'z',
	['ъ']= '\'',
	['ь']= '\'',
	['Э']= 'E',
	['э']= 'e',
--Slovak/Czech/Polish/Turkish:
	['ä']='a';
	['á']='a';
	['ą']='a';
	['ă']='a';
	['â']='a';
	['č']='c';
	['ć']='c';
	['ç']='c';
	['ď']='d';
	['đ']='d';
	['ě']='e';
	['é']='e';
	['ę']='e';
	['í']='i';
	['î']='i';
	['ĺ']='l';
	['ľ']='l';
	['ł']='l';
	['ň']='n';
	['ń']='n';
	['ó']='o';
	['ô']='o';
	['ö']='o';
	['ő']='o';
	['ř']='r';
	['ŕ']='r';
	['š']='s';
	['ś']='s';
	['ť']='t';
	['ú']='u';
	['ü']='u';
	['ű']='u';
	['ů']='u';
	['ý']='y';
	['ź']='z';
	['ž']='z';
	['ż']='z';
	
	['Ä']='A';
	['Á']='A';
	['Ą']='A';
	['Ă']='A';
	['Â']='A';
	['Č']='C';
	['Ć']='C';
	['Ç']='C';
	['Ď']='D';
	['Đ']='D';
	['Ě']='E';
	['É']='E';
	['Ę']='E';
	['Í']='I';
	['Î']='I';
	['Ĺ']='L';
	['Ľ']='L';
	['Ł']='L';
	['Ň']='N';
	['Ń']='N';
	['Ó']='O';
	['Ô']='O';
	['Ö']='O';
	['Ő']='O';
	['Ř']='R';
	['Ŕ']='R';
	['Š']='S';
	['Ś']='S';
	['Ť']='T';
	['Ú']='U';
	['Ü']='U';
	['Ű']='U';
	['Ů']='U';
	['Ý']='Y';
	['Ź']='Z';
	['Ž']='Z';
	['Ż']='Z';
	['€']='e';
};
function utf8clean(txt)
	--combos first:
	local combos = {
		[' Е']= ' Ye',
		['^Е']= 'Ye',
		[' е']= ' ye',
		['^е']= 'ye',
	};
	for i,v in pairs(combos) do
		txt=txt:gsub(i,v);
	end
	for i,v in pairs(UTF8Clean) do
		txt=txt:gsub(i,v);
	end
	return txt;
end
#pragma once

#define __stringa(x) stdext::_stringa(x)
#define __stringw(x) stdext::_stringw(x)
#define __tostra(x) stdext::tostra(x)
#define __tostrw(x) stdext::tostrw(x)
#define __chkfile(x) stdext::filechk(x)
#define __inifile stdext::inifile

namespace stdext
{
    struct strutil {
    protected:
        template<typename T>
        static bool compare_range_no_case(const T* base, size_t baseOffset, const T* find, size_t len)
        {
            for (size_t i = 0; i < len; ++i) {
                if (towlower(static_cast<unsigned short>(base[baseOffset + i])) !=
                    towlower(static_cast<unsigned short>(find[i])))
                    return false;
            }
            return true;
        }
        static bool compare_range_no_case_ansi(const char* base, size_t baseOffset, const char* find, size_t len)
        {
            for (size_t i = 0; i < len; ++i) {
                if (tolower(static_cast<unsigned char>(base[baseOffset + i])) !=
                    tolower(static_cast<unsigned char>(find[i])))
                    return false;
            }
            return true;
        }
    };

    class _stringa : public stra_t, private strutil {
    public:
        _stringa(const stra_t& s) : stra_t(s) {}
        _stringa(const char* s) : stra_t(s) {}

        bool startwith(const stra_t& prefix) const {
            if (this->length() < prefix.length()) return false;
            return compare_range_no_case_ansi(this->c_str(), 0, prefix.c_str(), prefix.length());
        }

        bool endswith(const stra_t& suffix) const {
            if (this->length() < suffix.length()) return false;
            return compare_range_no_case_ansi(this->c_str(), this->length() - suffix.length(),
                                              suffix.c_str(), suffix.length());
        }

        bool contains(const stra_t& root) const {
            if (root.empty()) return true;
            if (this->length() < root.length()) return false;

            size_t limit = this->length() - root.length();
            for (size_t i = 0; i <= limit; ++i) {
                if (compare_range_no_case_ansi(this->c_str(), i, root.c_str(), root.length()))
                    return true;
            }
            return false;
        }

        bool equal(const stra_t& str) const {
            if (this->length() != str.length()) return false;
            return compare_range_no_case_ansi(this->c_str(), 0, str.c_str(), str.length());
        }

        // New Replace Method
        _stringa replace(const stra_t& from, const stra_t& to) const {
            _stringa result(*this);
            if (from.empty()) return result;

            size_t pos = 0;
            // Case-insensitive search using the existing compare_range_no_case_ansi logic
            while (pos <= result.length() - from.length()) {
                if (compare_range_no_case_ansi(result.c_str(), pos, from.c_str(), from.length())) {
                    result.stra_t::replace(pos, from.length(), to);
                    pos += to.length(); // Advance past the newly inserted string

                    // Prevent out-of-bounds math if we've reached the end
                    if (result.length() < from.length()) break;
                } else {
                    ++pos;
                }
            }
            return result;
        }
    };

    class _stringw : public strw_t, private strutil {
    public:
        _stringw(const strw_t& s) : strw_t(s) {}
        _stringw(const wchar_t* s) : strw_t(s) {}

        bool startwith(const strw_t& prefix) const {
            if (this->length() < prefix.length()) return false;
            return compare_range_no_case(this->c_str(), 0, prefix.c_str(), prefix.length());
        }

        bool endswith(const strw_t& suffix) const {
            if (this->length() < suffix.length()) return false;
            return compare_range_no_case(this->c_str(), this->length() - suffix.length(),
                                         suffix.c_str(), suffix.length());
        }

        bool contains(const strw_t& root) const {
            if (root.empty()) return true;
            if (this->length() < root.length()) return false;

            size_t limit = this->length() - root.length();
            for (size_t i = 0; i <= limit; ++i) {
                if (compare_range_no_case(this->c_str(), i, root.c_str(), root.length()))
                    return true;
            }
            return false;
        }

        bool equal(const strw_t& str) const {
            if (this->length() != str.length()) return false;
            return compare_range_no_case(this->c_str(), 0, str.c_str(), str.length());
        }

        _stringw replace(const strw_t& from, const strw_t& to) const {
            _stringw result(*this);
            if (from.empty()) return result;

            size_t pos = 0;
            // Case-insensitive search using the existing compare_range_no_case logic
            while (pos <= result.length() - from.length()) {
                if (compare_range_no_case(result.c_str(), pos, from.c_str(), from.length())) {
                    result.strw_t::replace(pos, from.length(), to);
                    pos += to.length(); // Advance past the newly inserted string

                    if (result.length() < from.length()) break;
                } else {
                    ++pos;
                }
            }
            return result;
        }
    };

    class inifile {
    public:
        typedef std::map<stra_t, stra_t> map_str_str_t;
        typedef std::map<stra_t, map_str_str_t> m_data_t;
        typedef std::vector<stra_t> v_section_t;
        typedef std::map<stra_t, v_section_t> m_key_t;

    private:
        // Replaces the static member variable to fix LNK2001/LNK2019
        static const stra_t& get_static_null() {
            static const stra_t s_null = "NULL";
            return s_null;
        }

        // Internal helper for case-insensitive normalization
        stra_t to_lower(stra_t data) const {
            std::transform(data.begin(), data.end(), data.begin(), (int(*)(int))::tolower);
            return data;
        }

    public:
        inifile() {}
        ~inifile() {
            m_data.clear();
            m_sectionOrder.clear();
            m_keyOrder.clear();
        }

        bool load(const strw_t& filename) {
            m_file = filename;
            // VC80 conversion: wstring to ANSI string for ifstream
            stra_t ansiPath(filename.begin(), filename.end());
            return load(ansiPath);
        }

        bool load(const stra_t& filename) {
            m_data.clear();
            m_sectionOrder.clear();
            m_keyOrder.clear();

            // Use Windows API to open the file with full sharing permissions
            HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                       NULL, OPEN_EXISTING,
                                       FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE) return false;

            DWORD dwSize = GetFileSize(hFile, NULL);
            if (dwSize == 0xFFFFFFFF) {
                CloseHandle(hFile);
                return false;
            }

            std::vector<char> buffer(dwSize + 1);
            DWORD dwRead = 0;
            BOOL bResult = ReadFile(hFile, &buffer[0], dwSize,
                                    &dwRead, NULL);
            CloseHandle(hFile);

            if (!bResult) return false;

            buffer[dwSize] = '\0'; // Null terminate
            return parse(&buffer[0]);
        }

        bool save() {
            std::stringstream ss;

            for (size_t i = 0; i < m_sectionOrder.size(); ++i) {
                const stra_t& sectionOrig = m_sectionOrder[i];
                stra_t sL = to_lower(sectionOrig);

                ss << "[" << sectionOrig << "]\r\n";

                const std::vector<stra_t>& keys = m_keyOrder[sL];
                for (size_t j = 0; j < keys.size(); ++j) {
                    const stra_t& keyOrig = keys[j];
                    stra_t kL = to_lower(keyOrig);
                    ss << keyOrig << "=" << m_data[sL][kL] << "\r\n";
                }

                if (i < m_sectionOrder.size() - 1)
                    ss << "\r\n";
            }

            stra_t ansiPath(m_file.begin(), m_file.end());
            std::ofstream file(ansiPath.c_str(), std::ios::out | std::ios::binary);
            if (!file.is_open()) return false;
            file << ss.str();
            file.close();

            return true;
        }

        void setvalue(const stra_t& section, const stra_t& key, const stra_t& value) {
            stra_t sL = to_lower(section);
            stra_t kL = to_lower(key);

            // Store original casing for the first time we see a section/key
            if (m_data.find(sL) == m_data.end()) {
                m_sectionOrder.push_back(section);
            }

            if (m_data[sL].find(kL) == m_data[sL].end()) {
                m_keyOrder[sL].push_back(key);
            }

            m_data[sL][kL] = value;
        }

        const stra_t& getvalue(const stra_t& section, const stra_t& key, const stra_t& defaultValue = "NULL") {
            stra_t sL = to_lower(section);
            stra_t kL = to_lower(key);

            m_data_t::iterator itSect = m_data.find(sL);
            if (itSect != m_data.end()) {
                map_str_str_t::iterator itKey = itSect->second.find(kL);
                if (itKey != itSect->second.end()) {
                    return itKey->second;
                }
            }

            // Return internal static null if user didn't provide a specific default
            return (defaultValue == "NULL") ? get_static_null() : defaultValue;
        }

        int getint(const stra_t& section, const stra_t& key, int defaultValue = 0) {
            const stra_t& val = getvalue(section, key, "NULL");
            if (val == "NULL") return defaultValue;
            return atoi(val.c_str());
        }

        bool getbool(const stra_t& section, const stra_t& key, bool defaultValue = false) {
            const stra_t& val = getvalue(section, key, "NULL");
            if (val == "NULL") return defaultValue;

            stra_t vL = to_lower(val);
            if (vL == "true" || vL == "1" || vL == "yes" || vL == "on") return true;
            if (vL == "false" || vL == "0" || vL == "no" || vL == "off") return false;

            return defaultValue;
        }

        bool sectionexists(const stra_t& section) const {
            return m_data.find(to_lower(section)) != m_data.end();
        }

        bool keyexists(const stra_t& section, const stra_t& key) const {
            stra_t sL = to_lower(section);
            m_data_t::const_iterator itSect = m_data.find(sL);
            if (itSect != m_data.end()) {
                return itSect->second.find(to_lower(key)) != itSect->second.end();
            }
            return false;
        }

        void delsection(const stra_t& section) {
            stra_t sL = to_lower(section);
            m_data.erase(sL);
            m_keyOrder.erase(sL);

            for (v_section_t::iterator it = m_sectionOrder.begin(); it != m_sectionOrder.end(); ++it) {
                if (to_lower(*it) == sL) {
                    m_sectionOrder.erase(it);
                    break;
                }
            }
        }

        void delkey(const stra_t& section, const stra_t& key) {
            stra_t sL = to_lower(section);
            stra_t kL = to_lower(key);

            m_data_t::iterator itSect = m_data.find(sL);
            if (itSect != m_data.end()) {
                itSect->second.erase(kL);
            }

            m_key_t::iterator itKeyOrder = m_keyOrder.find(sL);
            if (itKeyOrder != m_keyOrder.end()) {
                v_section_t& keys = itKeyOrder->second;
                for (v_section_t::iterator it = keys.begin(); it != keys.end(); ++it) {
                    if (to_lower(*it) == kL) {
                        keys.erase(it);
                        break;
                    }
                }
            }
        }

    private:
        bool parse(const stra_t& content) {
            std::stringstream ss(content);
            stra_t line, currentSection;

            while (std::getline(ss, line)) {
                trim(line);
                if (line.empty() || line[0] == ';' || line[0] == '#') continue;

                if (line[0] == '[' && line[line.length() - 1] == ']') {
                    currentSection = line.substr(1, line.length() - 2);
                    trim(currentSection);
                } else {
                    size_t pos = line.find('=');
                    if (pos != stra_t::npos) {
                        stra_t key = line.substr(0, pos);
                        stra_t value = line.substr(pos + 1);
                        trim(key); trim(value);
                        setvalue(currentSection, key, value);
                    }
                }
            }
            return true;
        }

        static void trim(stra_t& s) {
            static const char* spaces = " \t\r\n";
            size_t first = s.find_first_not_of(spaces);
            if (first == stra_t::npos) { s.clear(); return; }
            size_t last = s.find_last_not_of(spaces);
            s = s.substr(first, (last - first + 1));
        }

        m_data_t m_data;
        v_section_t m_sectionOrder;
        m_key_t m_keyOrder;
        strw_t m_file;
    };

    inline _stringa tostra(const WCHAR* wstr)
    {
        if (!wstr || !wstr[0]) return stra_t("");

        // 1. Calculate the required buffer size
        int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr, -1,
                                              NULL, 0, NULL, NULL);

        if (size_needed <= 0) return stra_t("");

        // 2. Allocate buffer
        std::vector<char> buffer(size_needed);

        // 3. Perform the actual conversion
        WideCharToMultiByte(CP_ACP, 0, wstr, -1, &buffer[0],
                            size_needed, NULL, NULL);

        // 4. Convert to stra_t (size_needed includes null terminator, so we subtract 1)
        return stra_t(&buffer[0], size_needed - 1);
    }

    inline _stringw tostrw(const char* str)
    {
        if (!str || !str[0]) return strw_t(L"");

        // 1. Calculate the required buffer size (in wide characters)
        // CP_ACP uses the current system ANSI code page
        int size_needed = MultiByteToWideChar(CP_ACP, 0, str, -1,
                                              NULL, 0);

        if (size_needed <= 0) return strw_t(L"");

        // 2. Allocate buffer for wide characters
        std::vector<wchar_t> buffer(size_needed);

        // 3. Perform the actual conversion
        MultiByteToWideChar(CP_ACP, 0, str, -1, &buffer[0],
                            size_needed);

        // 4. Convert to strw_t
        // size_needed includes the null terminator, so we subtract 1 for the string object
        return strw_t(&buffer[0], size_needed - 1);
    }

    class filechk {
    private:
        stra_t  m_sName;
        strw_t m_wsName;
        bool m_isWide;

    public:
        // Constructor for standard strings
        filechk(const stra_t& filename) : m_sName(filename), m_isWide(false)
        {}

        // Constructor for wide strings (L"")
        filechk(const strw_t& filename) : m_wsName(filename), m_isWide(true)
        {}

        bool exists() const
        {
            if (m_isWide)
            {
                // VS2005 wifstream accepts const wchar_t*
                std::wifstream fileStream(m_wsName.c_str());
                return fileStream.good();
            }
            else
            {
                std::ifstream fileStream(m_sName.c_str());
                return fileStream.good();
            }
        }
    };

    class regedit {
    private:
        strw_t subKey;

    public:
        // Direct initialization
        regedit(const strw_t &sKey) : subKey(sKey)
        {}

        bool feature(
                const strw_t &featureName,
                const strw_t &appName,
                DWORD value)
        {
            HKEY hKey = NULL;

            // Ensure path separation logic
            strw_t fullPath = subKey;
            if (!fullPath.empty() && fullPath[fullPath.length() - 1] != L'\\') {
                fullPath += L"\\";
            }
            fullPath += featureName;

            // Open or create the subkey
            LONG result = RegCreateKeyExW(
                    HKEY_CURRENT_USER,
                    fullPath.c_str(),
                    0,
                    NULL,
                    REG_OPTION_NON_VOLATILE,
                    KEY_WRITE,
                    NULL,
                    &hKey,
                    NULL
            );

            if (result != ERROR_SUCCESS) {
                return false;
            }

            // Set the value
            result = RegSetValueExW(
                    hKey,
                    appName.c_str(),
                    0,
                    REG_DWORD,
                    reinterpret_cast<const BYTE*>(&value),
                    sizeof(DWORD)
            );

            // Always close the key
            RegCloseKey(hKey);

            return (result == ERROR_SUCCESS);
        }
    };

    template<typename Signature>
    class function;

    template<typename R, typename T1>
    class function<R(T1)> {
    private:
        typedef R (*FuncPtr)(T1);
        FuncPtr func_ptr;

    public:
        function() : func_ptr(NULL) {}
        function(FuncPtr ptr) : func_ptr(ptr) {}

        R operator()(T1 arg) {
            if (func_ptr) return func_ptr(arg);
            return R();
        }
    };

    inline const char* revit(const char *str)
    {
        char *reversed_str = const_cast<char *>(str);
        std::reverse(reversed_str, reversed_str + strlen(reversed_str));
        return reversed_str;
    }

    inline const char* getmodule()
    {
        static char szDllName[MAX_PATH];

        HMODULE hModule = NULL;
        TCHAR path[MAX_PATH], name[MAX_PATH];

        // Explicit cast to resolve the overload (pointing to this specific function)
        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR)(const char* (*)(void))getmodule, &hModule))
        {
            if (GetModuleFileName(hModule, path, MAX_PATH))
            {
                _tsplitpath(path, NULL, NULL, name, NULL);

                // Use the literal 25 (the size of our static buffer)
                _snprintf(szDllName, MAX_PATH, "%s.ini", name);
                return szDllName;
            }
        }

        return "NULL";
    }

    inline const char* getmodule(void* pRetAddr)
    {
        static char szModule[MAX_PATH];
        szModule[0] = '\0';
        HMODULE hModule = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR)pRetAddr, &hModule))
        {
            if (GetModuleFileName(hModule, szModule, MAX_PATH))
            {
                char* pszFileName = strrchr(szModule, '\\');
                return pszFileName ? pszFileName + 1 : szModule;
            }
        }

        return "NULL";
    }

    template <typename T>
    inline void dumpclass(const T& obj)
    {
        // Debug: Check what the compiler sees
        std::cout << "Attempting to write " << sizeof(T) << " bytes." << std::endl;

        std::stringstream ss;
        ss << &obj << ".bin";
        std::string filename = ss.str();

        std::ofstream outFile(filename.c_str(), std::ios::binary);
        if (outFile.is_open())
        {
            outFile.write(reinterpret_cast<const char*>(&obj), sizeof(T));
            outFile.close();
        }
    }

    inline std::vector<std::string> getsqlstring(const unsigned char* payload, size_t length)
    {
        std::vector<std::string> connectionStrings;
        const char* signature = "DRIVER=";
        size_t sigLength = 7;

        if (payload == NULL || length < sigLength + 2)
            return connectionStrings;

        // Scan for "DRIVER=" occurrences
        for (size_t i = 2; i <= length - sigLength; ++i)
        {
            if (std::memcmp(&payload[i], signature, sigLength) == 0)
            {
                // Parse Silkroad's 2-byte little-endian length prefix
                unsigned short stringLength = payload[i - 2] | (payload[i - 1] << 8);

                // Validation: ensure string length doesn't overrun packet payload allocation
                if ((i - 2) + 2 + stringLength <= length)
                {
                    std::string connStr(reinterpret_cast<const char*>(&payload[i]), stringLength);

                    // Validation filter to confirm valid SQL markers
                    if (connStr.find("SERVER=") != std::string::npos && connStr.find("DATABASE=") != std::string::npos)
                    {
                        connectionStrings.push_back(connStr);
                        i += stringLength - 1; // Advance scanning loop past processed string
                    }
                }
            }
        }

        return connectionStrings;
    }

    inline bool replacesqlinfo(std::string& connStr, const std::string& newUser, const std::string& newPwd)
    {
        bool foundAny = false;

        // 1. Handle UID= or User ID=
        std::string uidKey = "User ID=";
        size_t uidPos = connStr.find(uidKey);
        size_t keyLen = uidKey.length();

        if (uidPos == std::string::npos) {
            uidKey = "UID=";
            uidPos = connStr.find(uidKey);
            keyLen = uidKey.length();
        }

        if (uidPos != std::string::npos) {
            size_t uidEnd = connStr.find(";", uidPos);
            if (uidEnd == std::string::npos) {
                uidEnd = connStr.length();
            }
            connStr.replace(uidPos + keyLen, uidEnd - (uidPos + keyLen), newUser);
            foundAny = true;
        }

        // 2. Handle PWD= or Password=
        std::string pwdKey = "Password=";
        size_t pwdPos = connStr.find(pwdKey);
        size_t pwdKeyLen = pwdKey.length();

        if (pwdPos == std::string::npos) {
            pwdKey = "PWD=";
            pwdPos = connStr.find(pwdKey);
            pwdKeyLen = pwdKey.length();
        }

        if (pwdPos != std::string::npos) {
            size_t pwdEnd = connStr.find(";", pwdPos);
            if (pwdEnd == std::string::npos) {
                pwdEnd = connStr.length();
            }
            connStr.replace(pwdPos + pwdKeyLen, pwdEnd - (pwdPos + pwdKeyLen), newPwd);
            foundAny = true;
        }

        return foundAny;
    }
}

#include <iostream>
#include <map>
#include <regex>
#include <string>

std::unordered_map<std::string, std::vector<std::string>> countryLicensePatterns = {
    {"Albania", {"^(BC|BR|BZ|DI|DL|DR|DV|EL|ER|FR|GJ|GR|HS|KJ|KO|KR|KU|KV|LA|LB|LE|LU|MA|MK|MR|MT|PE|PG|PR|PU|SH|SK|SR|TP|TR|VL)[0-9]{4}[A-Z]{1,2}$"}}, // e.g., "AA123BB"
    {"Andorra", {"^[A-Z]{1,2}[0-9]{4}$"}}, // e.g., "A1234" or "AB1234"
    {"Austria", {"^\b(A|AM|B|BA|BB|BD|BG|BH|BL|BM|BN|BP|BR|BZ|DL|DO|E|EF|EU|FB|FE|FF|FK|FR|FV|FW|G|GB|GD|GF|GK|GM|GR|GS|GU|HA|HB|HE|HF|HL|HO|I|IL|IM|JE|JO|JU|JW|K|KB|KG|KI|KF|KK|KL|KO|KR|KS|KU|L|LA|LB|LE|LF|LI|LL|LN|LZ|MA|MD|ME|MI|MT|MU|MZ|N|ND|NK|O|OP|OW|P|PE|PL|PT|RA|RE|RI|RO|S|SB|SD|SE|SK|SL|SP|SO|SR|ST|SV|SW|SZ|T|TA|TD|TK|TU|UU|V|VB|VD|VI|VK|VL|VO|W|WB|WD|WK|WE|WL|WN|WO|WT|WU|WY|WZ|ZE|ZT|ZW)\d{1,5}[A-Z]{2,3}\b$"}}, // e.g., "W12345A"
    {"Belarus", {"^(E?\\d{4}[AEXIBCHKPTX]{2}\\d{1}|[AEXIBCHKPTX]{2}\\d{4}\\d{1}|[AEXIBCHKPTX]{1}\\d{4}[AEXIBCHKPTX]{1}\\d{1})$"}}, // e.g., "1234AB5" 
    {"Belgium", {"^[1-9][A-Z]{3}\\d{3}$"}}, // e.g., "1ABC123"
    {"Bosnia and Herzegovina", {"^(?:[A-EK-MO-T]\\d{2}[A-Z]\\d{3}|TA\\d{6}|TT\\d{6}|00000[A-Z]|00[ACME]\\d{3}|UN\\d{4}T|SFOR\\d{3}|EUPM\\d{4}|[A-Z]{2}\\d{4}|[A-Z]{2}\\d{4}|NATO\\d{3})$"}}, // e.g., "AA123AA"
    {"Bulgaria", {"^^(?:[AEKMNOPRSTUX]{1,2}\\d{4}[AEKMNOPRSTUX]{1,2}|[AEKMNOPRSTUX]{1,2}\\d{4}[AEKMNOPRSTUX]{2}|[A-EKMNOPRSTUX]{2}\\d{4}[A-EKMNOPRSTUX]{1,2}|[A-EKMNOPRSTUX]{2}\\d{4}[A-EKMNOPRSTUX]{2})$"}}, // e.g., "A1234AA"
    {"Croatia", {"^[ACEHIKLMNPRSTZ]{2}\\d{3,4}([ACEHIKLMNPRSTZ]{1,2})?$"}}, // e.g., "ZG1234AA"
    {"Cyprus", {"^[A-Z]{2,3}[0-9]{3}$"}}, // e.g., "ABC123"  
    {"Czech Republic", {"^([1-9]{1}[A-Z]{1}[A-Z]{1})\\s?\\d{4}$|^([1-9]{1}[A-Z]{1}[A-Z]{1})\\s?\\d{4}$|^([1-9]{1}[A-Z]{1})\\s?\\d{4}$|^[A-Z]{1}[A-Z]{1}[0-9]{1}[0-9]{1}$"}}, // e.g., "1234567"
    {"Denmark", {"^[A-Z]{2}\\d{2}\\d{3}$|^[A-Z]{2}\\d{2}\\d{2}$"}}, // e.g., "AB12345"
    {"Estonia", {"^\d{1,3}[A-Z]{3}$"}}, // e.g., "123ABC"
    {"Finland", {"^[A-Z]{2,3}\\d{1,3}$"}}, // e.g., "AB123" or "ABC12"
    {"France", {"^[A-Z]{2}[0-9]{3}[A-Z]{2}$"}}, // e.g., "AB123CD"
    {"Georgia", {"^[A-Z]{2}[0-9]{3}[A-Z]{2}$"}}, // e.g., "AB123CD"
    {"Greece", {"^([A-Z]{3}|[A-Z]{2}[A-Z]{1})([0-9]{3,4})$"}}, // e.g., "ABC1234"
    {"Hungary", {"^([AEIOU]{2}|[BCDFGHJKLMNPQRSTVWXYZ]{2})\\s[AEIOU]{2}\\s\\d{3}$"}}, // e.g., "ABC123"
    {"Iceland", {"^[A-Z]{2}(\\d{3}|[A-Z]\\d{1,2})$"}}, // e.g., "AB123"
    {"Ireland", {"^\\d{2,3}-[A-Z]{1,2}\\d{1,6}$"}}, // e.g., "12D12345"
    {"Italy", {"^[A-Z]{2}\\s\\d{3}[A-Z]{2}$"}}, // e.g., "AA123BB"
    {"Kosovo", {"^\\d{2}\\d{3}[A-Z]{2}$"}}, // e.g., "01123AB"
    {"Latvia", {"^[A-Z]{2}[0-9]{4}$"}}, // e.g., "AB1234" 
    {"Liechtenstein", {"^FL\\d{1,5}$"}}, // e.g., "12345"
    {"Lithuania", {"^[A-Z]{3}[0-9]{3}$"}}, // e.g., "ABC123"
    {"Luxembourg", {"^[A-Z]{2}\\d{2,4}$"}}, // e.g., "123456"
    {"Moldova", {"^[A-Z]{1,2}\\d{1,4}$"}}, // e.g., "ABC123"
    {"Monaco", {"^\d{2,4}$"}}, // e.g., "12AB34"
    {"Montenegro", {"^(AN|BA|BD|BP|BR|CT|DG|GS|HN|KL|KO|MK|NK|PG|PL|PT|PŽ|PV|RO|ŠN|TV|TZ|UL|ZT|ŽB)[A-Z]{2}\\d{2,5}$"}}, // e.g., "PG12345"
    {"Netherlands", {"^\\d{2}[A-Z]{2}[A-Z]{2}$|^[A-Z]{2}\\d{2}[A-Z]{2}$|^\\d{1}[A-Z]{3}\\d{2}$|^[A-Z]{2}\\d{3}[A-Z]{2}$|^\\d{1}[A-Z]{1}\\d{3}"}}, // e.g., "AB12CD"
    {"North Macedonia", {"^(BE|BT|DB|DE|DH|DK|GE|GV|KA|KI|KO|KR|KP|KS|KU|MB|MK|NE|OH|PE|PP|PS|RA|RE|SK|SN|SU|SR|ST|TE|VA|VE|VI|VV)\\d{4}[A-Z]{2}$"}}, // e.g., "SK1234AB"
    {"Norway", {"^^(AA|AB|AC|AD|AR|AS|AT|AU|AV|AW|DW|FL|ES|AX|AY|AZ|BA|AJ|AK|AL|AN|AP|FN|BW|AE|AF|AH|BL|BN|BP|BR|BS|BT|BU|BV|BX|BY|BZ|CA|CB|CV|CX|CY|CZ|CU|KE|KF|KH|KJ|KK|KL|KN|KP|KR|KS|KB|KC|KD|JU|JV|JX|JY|JZ|KA|KT|KU|KV|KX|KY|HJ|HK|HL|HN|HP|HR|HS|HT|HU|HV|HX|FB|HZ|JA|JB|PC|PD|PE|PF|PH|PJ|PK|RZ|SA|SB|PN|PP|PR|PS|PT|PU|PV|PW|UX|UY|UZ|VA|UR|US|UT|UU|UV|XV|XX|XY|XZ|FA|YN|YP|YR|YS|YU|YV|YX|RE|RF|RH|RJ|RK|RL|RN|RP|RR|RS|RT|RU|RV|RX|RY|RW|SN|SP|SR|SS|ST|SU|SV|SX|SY|SZ|TA|TB|TC|TD|TE|HW|TV|TX|TY|TZ|UC|UD|VB|VC|ZD|ZF|ZJ|EX|YZ|ZA|ZB|TW|ZX|FF|ZS|EY|ZZ|CW|FR|ZP|ZR|LE|ZC|ZE|ZH|ZK|ZL|FC|FP|VD|VE|VF|VH|VJ|VK|VL|VN|VP|VR|VS|VU|VV|NW|HF|HH|LH|LJ|LK|LL|LN|LP|LR|LD|ZV|ZW|ZT|ZU|ZV|ZY|ZK|ZL|ZN|FH)\\d{3,4}$"}}, // e.g., "AB12345"
    {"Poland", {"^(DB|D[AB|L|R|S|T|W|Z]|C[AB|G|S]|E[BL|K|S|W]|L[BL|C|D|G|I|K|L|M|O|P|R|S|W|Z]|F[GS|S|T]|K[B|K|S|T|W]|M[BR|S|L|Z]|N[BL|G|L|S|T]|P[A|B|L|N|O|P|S|T|W]|R[BA|BR|DZ|GR|L|P|SZ]|S[BE|BL|CE|DZ|G|K|W]|T[BU|JA|KI|SE|SK]|W[A|B|C|E|G|M|N|O|R|S|W|Z])\\d{3,5}$"}}, // e.g., "AB12345"
    {"Portugal", {"^\\d{2}[A-Z]{2}\\d{2}$"}}, // e.g., "1234AB" 
    {"Romania", {"^(B|AB|AG|AR|BC|BH|BN|BR|BT|BV|BZ|CJ|CL|CS|CT|CV|DB|DJ|GJ|GL|GR|HD|HR|IF|IL|IS|MH|MM|MS|NT|OT|PH|SB|SJ|SM|SV|TL|TM|TR|VL|VN|VS)\\d{2,3}[A-Z]{3}$"}}, // e.g., "B12ABC", "AB123XYZ", "B123ABC"
    {"Russia", {"^[ABEKMHOPCTYX]\\d{3} [ABEKMHOPCTYX]{2}$"}}, // e.g., "A123BC77"
    {"Serbia", {"^(AL|AR|AC|BB|BG|BO|BP|BT|BC|BU|BV|VA|VB|VL|VP|VR|VS|GM|DE|ZA|ZR|IN|IC|JA|KA|KC|KV|KG|KZ|KI|KL|KO|KS|LB|LE|LO|LU|NV|NG|NI|NP|NS|PA|PB|PZ|PI|PK|PN|PO|PP|PT|RA|RU|SA|SM|SV|SD|SJ|SO|SP|ST|SU|SC|TO|TS|TT|CU|UB|UE|CA|SA|SI)\d{3,4}[A-Z]{2}$"}}, // e.g., "BG123AB"
    {"Slovakia", {"^(BA|BD|BE|BI|BL|BT|BB|BC|BK|BJ|BN|BR|BS|BY|CA|DK|DS|DT|GA|GL|HC|HE|IL|KA|KE|KC|KI|KK|KM|KN|KS|LC|LE|LM|LV|LL|MA|MI|ML|MT|MY|NR|NI|NT|NM|NO|NZ|NC|PB|PD|PE|PK|PN|PO|PV|PS|PP|PT|PU|RA|RK|RS|RV|SA|SB|SC|SE|SI|SK|SL|SN|SO|SP|SV|TT|TA|TB|TN|TC|TE|TO|TR|TS|TV|VK|VT|ZA|ZI|ZL|ZC|ZH|ZM|ZV)\d{3}[A-Z]{2}$"}}, // e.g., "BA123CD"
    {"Slovenia", {"^(CE|GO|KK|KP|KR|LJ|MB|MS|NM|PO|SG)\d{3,4}[A-Z]{2}$"}}, // e.g., "LJ123AB"
    {"Spain", {"^[0-9]{4}[A-Z]{3}$"}}, // e.g., "1234ABC"
    {"Sweden", {"^[A-Z]{3}\d{3}$|^[A-Z]{3}\d{2}[A-Z]{1}$"}}, // e.g., "ABC123"
    {"Switzerland", {"^(AG|AI|AR|BE|BL|BS|FR|GE|GL|GR|JU|LU|NE|NW|OW|SG|SH|SO|SZ|TG|TI|UR|VD|VS|ZG|ZH)\d{1,6}$"}}, // e.g., "ZH123456"
    {"Ukraine", {"^(AA|KA|AB|KB|AC|KC|AE|KE|AH|KH|AI|KI|AK|KK|AM|KM|AO|KO|AP|KP|AT|KT|AX|KX|BA|HA|BB|HB|BC|HC|BE|HE|BH|HH|BI|HI|BK|HK|BM|HM|BO|HO|BP|HP|BT|HT|BX|HX|CA|IA|CB|IB|CC|IC|CE|IE|CH|IH|II)\d{4}[A-Z]{2}$"}}, // e.g., "AB1234CD"
};


// Function to match a license plate against the map of patterns
std::string identifyCountryFromPlate(const std::string& licensePlate) {
    for (const auto& entry : countryLicensePatterns) {
        for (const auto& pattern : entry.second) {
            // Check if the license plate matches any of the country's patterns
            std::regex re(pattern);
            if (std::regex_match(licensePlate, re)) {
                return entry.first; // Return the country name if the pattern matches
            }
        }
    }
    return "Unknown"; // Return "Unknown" if no match is found
}



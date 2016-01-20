using System.Collections.ObjectModel;
using System.Diagnostics;

namespace ClassLibrary
{
    static internal class LanguageAlias
    {
        /// <summary>
        /// converts an Iso three letter language id to the equivalent two letter code
        /// </summary>
        /// <param name="threeLetter"></param>
        /// <returns></returns>
        internal static string IsoThreeLetterToIsoTwoLetter(string threeLetter)
        {
            if (string.IsNullOrEmpty(threeLetter))
            {
                return string.Empty;
            }
            Debug.Assert(3 == threeLetter.Length, "Expected three letter ISO 639 language code");
            InitCultureTable();
            if (cultureTable != null)
            {
                string iso639Three = threeLetter.ToLowerInvariant();
                foreach (var cultureTableEntry in cultureTable)
                {
                    if (0 == string.CompareOrdinal(cultureTableEntry.ThreeLetterCode, iso639Three))
                    {
                        return cultureTableEntry.TwoLetterCode;
                    }
                }
            }
            return string.Empty;
        }

        /// <summary>
        /// converts an Iso two letter language id to the equivalent three letter code
        /// </summary>
        /// <param name="twoLetter"></param>
        /// <returns></returns>
        internal static string IsoTwoLetterToIsoThreeLetter(string twoLetter)
        {
            if (string.IsNullOrEmpty(twoLetter))
            {
                return string.Empty;
            }
            Debug.Assert(2 == twoLetter.Length, "Expected two letter ISO 639-1 language code");
            InitCultureTable();
            if (cultureTable != null)
            {
                string iso639Two = twoLetter.ToLowerInvariant();
                foreach (var cultureTableEntry in cultureTable)
                {
                    if (0 == string.CompareOrdinal(cultureTableEntry.TwoLetterCode, iso639Two))
                    {
                        return cultureTableEntry.ThreeLetterCode;
                    }
                }
            }
            return string.Empty;
        }

        /// <summary>
        /// lookup table entry
        /// </summary>
        private class CultureTableEntry
        {
            internal string TwoLetterCode { get; private set; }
            internal string ThreeLetterCode { get; private set; }
            internal CultureTableEntry(string twoLetterCode, string threeLetterCode)
            {
                this.TwoLetterCode = twoLetterCode;
                this.ThreeLetterCode = threeLetterCode;
            }
        }

        /// <summary>
        /// The lookup table
        /// </summary>
        private static Collection<CultureTableEntry> cultureTable;

        /// <summary>
        /// method to initialize the lookup table
        /// </summary>
        private static void InitCultureTable()
        {
            if (cultureTable == null)
            {
                cultureTable = new Collection<CultureTableEntry>();

                cultureTable.Add(new CultureTableEntry("af", "afr"));                // "af" - "Afrikaans"
                cultureTable.Add(new CultureTableEntry("am", "amh"));                // "am" - "Amharic"
                cultureTable.Add(new CultureTableEntry("ar", "ara"));                // "ar" - "Arabic"
                cultureTable.Add(new CultureTableEntry("arn", "arn"));                // "arn" - "Mapudungun"
                cultureTable.Add(new CultureTableEntry("as", "asm"));                // "as" - "Assamese"
                cultureTable.Add(new CultureTableEntry("az", "aze"));                // "az-Cyrl" - "Azeri (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("az", "aze"));                // "az-Latn" - "Azeri (Latin)"
                cultureTable.Add(new CultureTableEntry("az", "aze"));                // "az" - "Azeri"
                cultureTable.Add(new CultureTableEntry("ba", "bak"));                // "ba" - "Bashkir"
                cultureTable.Add(new CultureTableEntry("be", "bel"));                // "be" - "Belarusian"
                cultureTable.Add(new CultureTableEntry("bg", "bul"));                // "bg" - "Bulgarian"
                cultureTable.Add(new CultureTableEntry("bn", "bng"));                // "bn" - "Bengali"
                cultureTable.Add(new CultureTableEntry("bo", "bod"));                // "bo" - "Tibetan"
                cultureTable.Add(new CultureTableEntry("br", "bre"));                // "br" - "Breton"
                cultureTable.Add(new CultureTableEntry("bs", "bsb"));                // "bs-Latn" - "Bosnian (Latin)"
                cultureTable.Add(new CultureTableEntry("bs", "bsb"));                // "bs" - "Bosnian"
                cultureTable.Add(new CultureTableEntry("bs", "bsc"));                // "bs-Cyrl" - "Bosnian (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("ca", "cat"));                // "ca" - "Catalan"
                cultureTable.Add(new CultureTableEntry("co", "cos"));                // "co" - "Corsican"
                cultureTable.Add(new CultureTableEntry("cs", "ces"));                // "cs" - "Czech"
                cultureTable.Add(new CultureTableEntry("cy", "cym"));                // "cy" - "Welsh"
                cultureTable.Add(new CultureTableEntry("da", "dan"));                // "da" - "Danish"
                cultureTable.Add(new CultureTableEntry("de", "deu"));                // "de" - "German"
                cultureTable.Add(new CultureTableEntry("dsb", "dsb"));                // "dsb" - "Lower Sorbian"
                cultureTable.Add(new CultureTableEntry("dv", "div"));                // "dv" - "Divehi"
                cultureTable.Add(new CultureTableEntry("el", "ell"));                // "el" - "Greek"
                cultureTable.Add(new CultureTableEntry("en", "eng"));                // "en" - "English"
                cultureTable.Add(new CultureTableEntry("es", "spa"));                // "es" - "Spanish"
                cultureTable.Add(new CultureTableEntry("et", "est"));                // "et" - "Estonian"
                cultureTable.Add(new CultureTableEntry("eu", "eus"));                // "eu" - "Basque"
                cultureTable.Add(new CultureTableEntry("fa", "fas"));                // "fa" - "Persian"
                cultureTable.Add(new CultureTableEntry("fi", "fin"));                // "fi" - "Finnish"
                cultureTable.Add(new CultureTableEntry("fil", "fil"));                // "fil" - "Filipino"
                cultureTable.Add(new CultureTableEntry("fo", "fao"));                // "fo" - "Faroese"
                cultureTable.Add(new CultureTableEntry("fr", "fra"));                // "fr" - "French"
                cultureTable.Add(new CultureTableEntry("fy", "fry"));                // "fy" - "Frisian"
                cultureTable.Add(new CultureTableEntry("ga", "gle"));                // "ga" - "Irish"
                cultureTable.Add(new CultureTableEntry("gd", "gla"));                // "gd" - "Scottish Gaelic"
                cultureTable.Add(new CultureTableEntry("gl", "glg"));                // "gl" - "Galician"
                cultureTable.Add(new CultureTableEntry("gsw", "gsw"));                // "gsw" - "Alsatian"
                cultureTable.Add(new CultureTableEntry("gu", "guj"));                // "gu" - "Gujarati"
                cultureTable.Add(new CultureTableEntry("ha", "hau"));                // "ha-Latn" - "Hausa (Latin)"
                cultureTable.Add(new CultureTableEntry("ha", "hau"));                // "ha" - "Hausa"
                cultureTable.Add(new CultureTableEntry("he", "heb"));                // "he" - "Hebrew"
                cultureTable.Add(new CultureTableEntry("hi", "hin"));                // "hi" - "Hindi"
                cultureTable.Add(new CultureTableEntry("hr", "hrv"));                // "hr" - "Croatian"
                cultureTable.Add(new CultureTableEntry("hsb", "hsb"));                // "hsb" - "Upper Sorbian"
                cultureTable.Add(new CultureTableEntry("hu", "hun"));                // "hu" - "Hungarian"
                cultureTable.Add(new CultureTableEntry("hy", "hye"));                // "hy" - "Armenian"
                cultureTable.Add(new CultureTableEntry("id", "ind"));                // "id" - "Indonesian"
                cultureTable.Add(new CultureTableEntry("ig", "ibo"));                // "ig" - "Igbo"
                cultureTable.Add(new CultureTableEntry("ii", "iii"));                // "ii" - "Yi"
                cultureTable.Add(new CultureTableEntry("is", "isl"));                // "is" - "Icelandic"
                cultureTable.Add(new CultureTableEntry("it", "ita"));                // "it" - "Italian"
                cultureTable.Add(new CultureTableEntry("iu", "iku"));                // "iu-Cans" - "Inuktitut (Syllabics)"
                cultureTable.Add(new CultureTableEntry("iu", "iku"));                // "iu-Latn" - "Inuktitut (Latin)"
                cultureTable.Add(new CultureTableEntry("iu", "iku"));                // "iu" - "Inuktitut"
                cultureTable.Add(new CultureTableEntry("iv", "ivl"));                // "" - "Invariant Language (Invariant Country)"
                cultureTable.Add(new CultureTableEntry("ja", "jpn"));                // "ja" - "Japanese"
                cultureTable.Add(new CultureTableEntry("ka", "kat"));                // "ka" - "Georgian"
                cultureTable.Add(new CultureTableEntry("kk", "kaz"));                // "kk" - "Kazakh"
                cultureTable.Add(new CultureTableEntry("kl", "kal"));                // "kl" - "Greenlandic"
                cultureTable.Add(new CultureTableEntry("km", "khm"));                // "km" - "Khmer"
                cultureTable.Add(new CultureTableEntry("kn", "kan"));                // "kn" - "Kannada"
                cultureTable.Add(new CultureTableEntry("ko", "kor"));                // "ko" - "Korean"
                cultureTable.Add(new CultureTableEntry("kok", "kok"));                // "kok" - "Konkani"
                cultureTable.Add(new CultureTableEntry("ky", "kir"));                // "ky" - "Kyrgyz"
                cultureTable.Add(new CultureTableEntry("lb", "ltz"));                // "lb" - "Luxembourgish"
                cultureTable.Add(new CultureTableEntry("lo", "lao"));                // "lo" - "Lao"
                cultureTable.Add(new CultureTableEntry("lt", "lit"));                // "lt" - "Lithuanian"
                cultureTable.Add(new CultureTableEntry("lv", "lav"));                // "lv" - "Latvian"
                cultureTable.Add(new CultureTableEntry("mi", "mri"));                // "mi" - "Maori"
                cultureTable.Add(new CultureTableEntry("mk", "mkd"));                // "mk" - "Macedonian (FYROM)"
                cultureTable.Add(new CultureTableEntry("ml", "mym"));                // "ml" - "Malayalam"
                cultureTable.Add(new CultureTableEntry("mn", "mon"));                // "mn-Cyrl" - "Mongolian (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("mn", "mon"));                // "mn-Mong" - "Mongolian (Traditional Mongolian)"
                cultureTable.Add(new CultureTableEntry("mn", "mon"));                // "mn" - "Mongolian"
                cultureTable.Add(new CultureTableEntry("moh", "moh"));                // "moh" - "Mohawk"
                cultureTable.Add(new CultureTableEntry("mr", "mar"));                // "mr" - "Marathi"
                cultureTable.Add(new CultureTableEntry("ms", "msa"));                // "ms" - "Malay"
                cultureTable.Add(new CultureTableEntry("mt", "mlt"));                // "mt" - "Maltese"
                cultureTable.Add(new CultureTableEntry("nb", "nob"));                // "nb" - "Norwegian (Bokmål)"
                cultureTable.Add(new CultureTableEntry("nb", "nob"));                // "no" - "Norwegian"
                cultureTable.Add(new CultureTableEntry("ne", "nep"));                // "ne" - "Nepali"
                cultureTable.Add(new CultureTableEntry("nl", "nld"));                // "nl" - "Dutch"
                cultureTable.Add(new CultureTableEntry("nn", "nno"));                // "nn" - "Norwegian (Nynorsk)"
                cultureTable.Add(new CultureTableEntry("nso", "nso"));                // "nso" - "Sesotho sa Leboa"
                cultureTable.Add(new CultureTableEntry("oc", "oci"));                // "oc" - "Occitan"
                cultureTable.Add(new CultureTableEntry("or", "ori"));                // "or" - "Oriya"
                cultureTable.Add(new CultureTableEntry("pa", "pan"));                // "pa" - "Punjabi"
                cultureTable.Add(new CultureTableEntry("pl", "pol"));                // "pl" - "Polish"
                cultureTable.Add(new CultureTableEntry("prs", "prs"));                // "prs" - "Dari"
                cultureTable.Add(new CultureTableEntry("ps", "pus"));                // "ps" - "Pashto"
                cultureTable.Add(new CultureTableEntry("pt", "por"));                // "pt" - "Portuguese"
                cultureTable.Add(new CultureTableEntry("qut", "qut"));                // "qut" - "K'iche"
                cultureTable.Add(new CultureTableEntry("quz", "qub"));                // "quz" - "Quechua"
                cultureTable.Add(new CultureTableEntry("rm", "roh"));                // "rm" - "Romansh"
                cultureTable.Add(new CultureTableEntry("ro", "ron"));                // "ro" - "Romanian"
                cultureTable.Add(new CultureTableEntry("ru", "rus"));                // "ru" - "Russian"
                cultureTable.Add(new CultureTableEntry("rw", "kin"));                // "rw" - "Kinyarwanda"
                cultureTable.Add(new CultureTableEntry("sa", "san"));                // "sa" - "Sanskrit"
                cultureTable.Add(new CultureTableEntry("sah", "sah"));                // "sah" - "Yakut"
                cultureTable.Add(new CultureTableEntry("se", "sme"));                // "se" - "Sami (Northern)"
                cultureTable.Add(new CultureTableEntry("si", "sin"));                // "si" - "Sinhala"
                cultureTable.Add(new CultureTableEntry("sk", "slk"));                // "sk" - "Slovak"
                cultureTable.Add(new CultureTableEntry("sl", "slv"));                // "sl" - "Slovenian"
                cultureTable.Add(new CultureTableEntry("sma", "smb"));                // "sma" - "Sami (Southern)"
                cultureTable.Add(new CultureTableEntry("smj", "smk"));                // "smj" - "Sami (Lule)"
                cultureTable.Add(new CultureTableEntry("smn", "smn"));                // "smn" - "Sami (Inari)"
                cultureTable.Add(new CultureTableEntry("sms", "sms"));                // "sms" - "Sami (Skolt)"
                cultureTable.Add(new CultureTableEntry("sq", "sqi"));                // "sq" - "Albanian"
                cultureTable.Add(new CultureTableEntry("sr", "srp"));                // "sr-Cyrl" - "Serbian (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("sr", "srp"));                // "sr-Latn" - "Serbian (Latin)"
                cultureTable.Add(new CultureTableEntry("sr", "srp"));                // "sr" - "Serbian"
                cultureTable.Add(new CultureTableEntry("sv", "swe"));                // "sv" - "Swedish"
                cultureTable.Add(new CultureTableEntry("sw", "swa"));                // "sw" - "Kiswahili"
                cultureTable.Add(new CultureTableEntry("syr", "syr"));                // "syr" - "Syriac"
                cultureTable.Add(new CultureTableEntry("ta", "tam"));                // "ta" - "Tamil"
                cultureTable.Add(new CultureTableEntry("te", "tel"));                // "te" - "Telugu"
                cultureTable.Add(new CultureTableEntry("tg", "tgk"));                // "tg-Cyrl" - "Tajik (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("tg", "tgk"));                // "tg" - "Tajik"
                cultureTable.Add(new CultureTableEntry("th", "tha"));                // "th" - "Thai"
                cultureTable.Add(new CultureTableEntry("tk", "tuk"));                // "tk" - "Turkmen"
                cultureTable.Add(new CultureTableEntry("tn", "tsn"));                // "tn" - "Setswana"
                cultureTable.Add(new CultureTableEntry("tr", "tur"));                // "tr" - "Turkish"
                cultureTable.Add(new CultureTableEntry("tt", "tat"));                // "tt" - "Tatar"
                cultureTable.Add(new CultureTableEntry("tzm", "tzm"));                // "tzm-Latn" - "Tamazight (Latin)"
                cultureTable.Add(new CultureTableEntry("tzm", "tzm"));                // "tzm" - "Tamazight"
                cultureTable.Add(new CultureTableEntry("ug", "uig"));                // "ug" - "Uyghur"
                cultureTable.Add(new CultureTableEntry("uk", "ukr"));                // "uk" - "Ukrainian"
                cultureTable.Add(new CultureTableEntry("ur", "urd"));                // "ur" - "Urdu"
                cultureTable.Add(new CultureTableEntry("uz", "uzb"));                // "uz-Cyrl" - "Uzbek (Cyrillic)"
                cultureTable.Add(new CultureTableEntry("uz", "uzb"));                // "uz-Latn" - "Uzbek (Latin)"
                cultureTable.Add(new CultureTableEntry("uz", "uzb"));                // "uz" - "Uzbek"
                cultureTable.Add(new CultureTableEntry("vi", "vie"));                // "vi" - "Vietnamese"
                cultureTable.Add(new CultureTableEntry("wo", "wol"));                // "wo" - "Wolof"
                cultureTable.Add(new CultureTableEntry("xh", "xho"));                // "xh" - "isiXhosa"
                cultureTable.Add(new CultureTableEntry("yo", "yor"));                // "yo" - "Yoruba"
                cultureTable.Add(new CultureTableEntry("zh", "zho"));                // "zh-CHS" - "Chinese (Simplified) Legacy"
                cultureTable.Add(new CultureTableEntry("zh", "zho"));                // "zh-CHT" - "Chinese (Traditional) Legacy"
                cultureTable.Add(new CultureTableEntry("zh", "zho"));                // "zh-Hans" - "Chinese (Simplified)"
                cultureTable.Add(new CultureTableEntry("zh", "zho"));                // "zh-Hant" - "Chinese (Traditional)"
                cultureTable.Add(new CultureTableEntry("zh", "zho"));                // "zh" - "Chinese"
                cultureTable.Add(new CultureTableEntry("zu", "zul"));                // "zu" - "isiZulu"
            }
        }
    }
}
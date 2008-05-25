#include "../libFireBird.h"

char *iso639_1 (int OSDLan)
{
  switch (OSDLan)
  {
    case LAN_English:
      return "en";

    case LAN_French:
      return "fr";

    case LAN_German:
      return "de";

    case LAN_Italy:
      return "it";

    case LAN_Spain:
      return "es";

    case LAN_Arab:
      return "ar";

    case LAN_Greek:
      return "el";

    case LAN_Turk:
      return "tr";

    case LAN_Danish:
      return "da";

    case LAN_Swedish:
      return "sv";

    case LAN_Norwegian:
      return "no";

    case  LAN_Dutch:
      return "nl";

    case LAN_Russian:
      return "ru";

    case LAN_Polish:
      return "pl";

    case LAN_Persian:
      return "fa";

    case LAN_Suomi:
      return "fi";

    case LAN_Slovak:
      return "sk";

    case LAN_Thai:
      return "th";

    case LAN_Czech:
      return "cs";

    case LAN_Bulgarian:
      return "bg";

    default:
      return NULL;
  }
}

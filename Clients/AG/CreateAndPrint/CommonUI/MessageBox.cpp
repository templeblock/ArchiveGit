#include "stdafx.h"
#include "MessageBox.h"

#if (HPAPP)
	#include "Utility.h"
#else
	#include "AGDoc.h"
#endif

#if !(XMLUTIL || CTPUTIL)
	#include "HTMLDialog.h"
#endif

#if (XMLUTIL)
	#include "..\Xml\XML.h"
	extern CXMLApp theApp;
#endif

#if (CTPUTIL)
	#include "..\CtpUtil\CtpUtil.h"
	extern CCTPUtilApp theApp;
#endif

#define ERROR_NO_OBJECT_SELECTED	"You must select one of the objects on the project page before using this function."
#define ERROR_OBJECT_LOCKED			"You cannot edit this image because it is locked."

HWND CMessageBox::m_hWndParent;
CString CMessageBox::m_strTitle;
CString CMessageBox::m_strHtmlFile;

/////////////////////////////////////////////////////////////////////////////
void CMessageBox::SetDefaults(HWND hWndParent, LPCSTR strTitle, LPCSTR strHtmlFile)
{
	m_hWndParent = hWndParent;
	m_strTitle = strTitle;
	m_strHtmlFile = strHtmlFile;
}

/////////////////////////////////////////////////////////////////////////////
#if !(HPAPP)
void DoUKTranslation(CString& strMessage)
{ // See the complete (compleat) list of UK-US words pairs at the bottom of this file
#if (CTPUTIL)
	return;
#else
	char* pTarget;
	pTarget = "color";
	if (strMessage.Find(pTarget))
		Replace(strMessage, pTarget, "colour");
	pTarget = "labor";
	if (strMessage.Find(pTarget))
		Replace(strMessage, pTarget, "labour");
	pTarget = "personaliz";
	if (strMessage.Find(pTarget))
		Replace(strMessage, pTarget, "personalis");
	pTarget = "quantiz";
	if (strMessage.Find(pTarget))
		Replace(strMessage, pTarget, "quantis");
#endif CTPUTIL
}
#endif

/////////////////////////////////////////////////////////////////////////////
int CMessageBox::Message(UINT uMessage, DWORD dwStyle, UINT uIcon)
{
	CString strMessage;
	strMessage.Format(uMessage);
	return Message(strMessage, dwStyle, uIcon);
}

/////////////////////////////////////////////////////////////////////////////
int CMessageBox::Message(LPCTSTR pMessage, DWORD dwStyle, UINT uIcon)
{
	CString strMessage = pMessage;
#if !(HPAPP)
	if (CAGDocSheetSize::GetMetric())
		DoUKTranslation(strMessage);
#endif

	// Using GetActiveWindow() for the parent is a problem during WM_DESTROY,
	// so add the MB_SYSTEMMODAL flag if we are on the way out
	HWND hWndParent = NULL;
	if (!(dwStyle & MB_SYSTEMMODAL))
	{
		// See if our window is still valid
		if (!::IsWindow(m_hWndParent))
			m_hWndParent = NULL;
		hWndParent = (m_hWndParent ? m_hWndParent : ::GetActiveWindow());
	}

#if !(XMLUTIL || CTPUTIL)
	DWORD dwType = dwStyle & MB_TYPEMASK;
	if (!dwType)
		dwType = MB_OK;

	DWORD dwIcon = dwStyle & MB_ICONMASK;
	if (uIcon)
	{
		dwIcon |= MB_USERICON;
	}
	else
	if (!dwIcon)
	{
		if (strMessage.Find('?') >= 0)
			dwIcon |= MB_ICONQUESTION;
		else
		if (strMessage.Find("Error") >= 0)
			dwIcon |= MB_ICONERROR;
		else
			dwIcon |= MB_ICONINFORMATION; //j MB_ICONWARNING;
	}

	dwStyle |= dwType;
	dwStyle |= dwIcon;

	do // a convenient block to break out of
	{ // Let's try to create an HTML MessageBox
		CRect rMessageBox;
		if (!ComputeMessageBoxRect(rMessageBox, strMessage))
			break; // it failed, so try the old fashion way...

		CString strHtmlFile = m_strHtmlFile;
		if (strHtmlFile.IsEmpty())
			strHtmlFile = "MessageBox.htm";

		CHtmlDialog dlg(strHtmlFile, true/*bRes*/, hWndParent);
		dlg.SetWindowSize(rMessageBox.Width(), rMessageBox.Height());
		dlg.SetResizable(false);
		dlg.SetScrollbars(false);
		dlg.SetValue("title", m_strTitle);
		dlg.SetValue("message", strMessage);

		if (dwIcon == MB_ICONWARNING)
			dlg.SetValue("icon", "IconWarning.gif");
		else
		if (dwIcon == MB_ICONERROR)
			dlg.SetValue("icon", "IconError.gif");
		else
		if (dwIcon == MB_ICONINFORMATION || dwIcon == MB_USERICON)
			dlg.SetValue("icon", "IconInfo.gif");
		else
		if (dwIcon == MB_ICONQUESTION)
			dlg.SetValue("icon", "IconQuestion.gif");

		if (dwType == MB_OK || dwType == MB_OKCANCEL)
			dlg.SetValue("button", "OK");
		if (dwType == MB_YESNO || dwType == MB_YESNOCANCEL)
			dlg.SetValue("button", "Yes");
		if (dwType == MB_YESNO || dwType == MB_YESNOCANCEL)
			dlg.SetValue("button", "No");
		if (dwType == MB_ABORTRETRYIGNORE)
			dlg.SetValue("button", "Abort");
		if (dwType == MB_ABORTRETRYIGNORE || dwType == MB_RETRYCANCEL)
			dlg.SetValue("button", "Retry");
		if (dwType == MB_ABORTRETRYIGNORE)
			dlg.SetValue("button", "Ignore");
		if (dwType == MB_OKCANCEL || dwType == MB_YESNOCANCEL || dwType == MB_RETRYCANCEL)
			dlg.SetValue("button", "Cancel");
		if (!dlg.DoModal())
			break; // it failed, so try the old fashion way...

		CString strReturnValue = dlg.GetReturnString();
		if (strReturnValue == "OK")
			return IDOK;
		else
		if (strReturnValue == "Yes")
			return IDYES;
		else
		if (strReturnValue == "No")
			return IDNO;
		else
		if (strReturnValue == "Abort")
			return IDABORT;
		else
		if (strReturnValue == "Retry")
			return IDRETRY;
		else
		if (strReturnValue == "Ignore")
			return IDIGNORE;
		else
		if (strReturnValue == "Cancel")
			return IDCANCEL;
		else
			return IDCANCEL;
	} while (0);
#endif

	// A traditional MessageBox if the HTML version fails...
#if (XMLUTIL || CTPUTIL)
	HMODULE hModule = GetModuleHandle(theApp.m_pszExeName);
#else
	HMODULE hModule = _AtlBaseModule.GetResourceInstance();
#endif

	MSGBOXPARAMS Params;
	::ZeroMemory(&Params, sizeof(Params));
	Params.cbSize = sizeof(MSGBOXPARAMS);
	Params.hwndOwner = hWndParent;
	Params.hInstance = hModule;
	Params.lpszText = strMessage;
	Params.lpszCaption = m_strTitle;
	Params.dwStyle = dwStyle;
	Params.lpszIcon = MAKEINTRESOURCE(uIcon);
	Params.dwContextHelpId = NULL;
	Params.lpfnMsgBoxCallback = NULL;
	Params.dwLanguageId = NULL;
	return ::MessageBoxIndirect(&Params);
}

/////////////////////////////////////////////////////////////////////////////
bool IsIE7Installed()
{
	#define REGKEY_IEVERSION "Software\\Microsoft\\Internet Explorer"
	#define REGVAL_IEVERSION "Version"

	CRegKey regkey;
	if (regkey.Open(HKEY_LOCAL_MACHINE, REGKEY_IEVERSION) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	szBuffer[0] = 0;
	if (regkey.QueryStringValue(REGVAL_IEVERSION, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

	double fVersion = atof(szBuffer);
	return (fVersion >= 7);
}

/////////////////////////////////////////////////////////////////////////////
bool CMessageBox::ComputeMessageBoxRect(CRect& rMessageBox, LPCTSTR pMessage)
{
	// These values should match the design driven by MessageBox.htm
	#define MSGBOX_MARGIN_L		(14+32+14)
	#define MSGBOX_MARGIN_R		(44)
	#define MSGBOX_MARGIN_T		(50+5)
	#define MSGBOX_MARGIN_B		(12+24+12+12)
	#define MSGBOX_MINMSGHEIGHT	(32)
	#define MSGBOX_FONTHEIGHT 	15 // 1.25% of the HTML message font (e.g., if 12px use 15 here)
	#define MSGBOX_FONTFACE		"Arial"

	HDC hDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	if (!hDC)
		return false;

	LOGFONT Font;
	memset(&Font, 0, sizeof(Font));
	Font.lfHeight = MSGBOX_FONTHEIGHT;
	Font.lfWidth = 0;
	::strcpy(Font.lfFaceName, MSGBOX_FONTFACE);

	HFONT hFont = ::CreateFontIndirect(&Font);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

	CSize SizeMargin(MSGBOX_MARGIN_L + MSGBOX_MARGIN_R, MSGBOX_MARGIN_T + MSGBOX_MARGIN_B);

	// For IE7 or later, dialogHeight and dialogWidth are restricted to the height and width
	// of the content area, and no longer includes the height and width of the frame.
	bool bIE7 = IsIE7Installed();
	if (!bIE7)
	{
		SizeMargin.cx += (2 * ::GetSystemMetrics(SM_CXSIZEFRAME));
		SizeMargin.cy += (2 * ::GetSystemMetrics(SM_CYSIZEFRAME)) + ::GetSystemMetrics(SM_CYCAPTION);
	}

	int nMaxWidth = ::GetSystemMetrics(SM_CXSCREEN) / 2;
	int nMaxHeight = nMaxWidth;
	nMaxWidth -= SizeMargin.cx;
	nMaxHeight -= SizeMargin.cy;

	CRect rMessage(0, 0, nMaxWidth, nMaxHeight);
	::DrawText(hDC, pMessage, -1, &rMessage, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);

	if (HEIGHT(rMessage) < MSGBOX_MINMSGHEIGHT)
		rMessage.bottom = rMessage.top + MSGBOX_MINMSGHEIGHT;

	rMessageBox.SetRect(0, 0, rMessage.Width() + SizeMargin.cx, rMessage.Height() + SizeMargin.cy);

	::SelectObject(hDC, hOldFont);
	::DeleteObject(hFont);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CMessageBox::NoObjectSelected()
{
	Message(ERROR_NO_OBJECT_SELECTED);
}

/////////////////////////////////////////////////////////////////////////////
void CMessageBox::ObjectIsLocked()
{
	Message(ERROR_OBJECT_LOCKED);
}

/* From http://www.xpdnc.com/moreinfo/orlabour.html
AMERICAN BRITISH

abridgment abridgement
accouterment accoutrement
acknowledgment acknowledgement
advertize advertise
afterward afterwards
agonize agonise
airplane aeroplane
almshouse almhouse
aluminum aluminium
ambiance ambience
ameba amoeba
amphitheater amphitheatre
analog analogue
analyze analyse
analyzed analysed
analyzer analyser
analyzing analysing
anemia anaemia
anemic anaemic
anesthesia anaesthesia
anesthetic anaesthetic
anesthetizing anaesthetising
annex annexe
anodizing anodising
antiaircraft anti-aircraft
apologize apologise
apothegm apophthegm
arbor arbour
archeology archaeology
archiepiscopal archipiscopal
ardor ardour
armor armour
armored armoured
armorer armourer
armoring armouring
armory armoury
artifact artefact
atchoo atishoo
atomizer atomiser
atomizing atomising
ax axe
B.S. B.Sc.
backward backwards
baptize baptise
barbecue barbeque
bark barque
battle-ax battleaxe
behavior behaviour
behavioral behavioural
behoove behove
Benedictine Benedectine
beside besides
bisulfate bisulphate
bookkeeper book-keeper
boro borough
bylaw bye law
caesarean caesarian
caldron cauldron
caliber calibre
caliper calliper
calisthenics callisthenics
canceling cancelling
candor candour
carbonization carbonisation
carburetor carburetter
carburetor carburettor
carburizing carburising
carcass carcas
catalog catalogue
catalyze catalyse
cauterization cauterisation
cauterizing cauterising
center centre
centerboard centreboard
centerfold centrefold
centering centring
centimeter centimetre
centralize centralise
cesarean cesarian
cesium caesium
channeled channelled
characterize characterise
check cheque
checker chequer
chili chilli
Christianized Christianised
cigaret cigarette
citrus citrous
civilization civilisation
civilize civilise
clamor clamour
clangor clangour
collectible collectable
colonnaded collonaded
color colour
colored coloured
colorful colourful
coloring colouring
colter coulter
commission commision
complete compleat
conjuror conjurer
counselor counsellor
cozies cosies
cozy cosy
criticize criticise
cross-linking crosslinking
crystallizing crystallising
curb kerb
cyclization cyclisation
cyclopedia cyclopaedia
daydream day-dream
decarburizing decarburising
decolorizing decolourising
defense defence
deflexion deflection
demeanor demeanour
deodorizing deodorising
depolarizing depolarising
depolymerizing depolymerising
desensitizer desensitiser
desulfurizing desulfurising
dialling dialing
dialog dialogue
dialyze dialyse
diarrhea diarrhoea
dike dyke
disk disc
disulfide disulphide
dolor dolour
donut doughnut
doodad doodah
draft draught
drypoint dry-point
economizer economiser
edema oedema
emphasize emphasise
employe employee
enameled enamelled
enameling enamelling
enamor enamour
enclosure inclosure
encyclopedia encyclopaedia
endeavor endeavour
enology oenology
enroll enrol
enrollment enrolment
ensorcell ensorcel
ensure insure
eon aeon
equaled equalled
equalizing equalising
esophageal oesophagal
esophagoscope oesophagoscope
esophagus oesophagus
estrous oestrous
estrus oestrous
etiology aetiology
eulogize eulogise
fairy tale fairy-tale
fantasize fantasise
favor favour
favorite favourite
favoritism favouritism
fecal faecal
fertilization fertilisation
fertilizing fertilising
fertlizer fertiliser
fervor fervour
fetus foetus
fiber fibre
fiberglass fibreglass
fibers fibres
flavor flavour
flavoring flavouring
fledgling fledgeling
flower girl flower-girl
flower pot flower-pot
fluidized fluidised
fluidizing fluidising
forward forwards
fresh water freshwater
fulfill fulfil
fulfillment fulfilment
fulfll fulfil
furor furore
gage gauge
galoshes goloshes
galvanizing galvanising
goiter goitre
gonorrhea gonorrhoea
gram gramme
graphitizing graphitising
gray grey
greenhouse glasshouse
groins groynes
gynecology gynaecology
harbor harbour
harmonize harmonise
hauler haulier
hematein haematein
hemoglobin haemoglobin
hemophilia haemophilia
hemorrhage haemorrage
hemorrhage haemorrhage
hemorrhoid haemorrhoid
heyday heydey
homeopath homoeopath
homeostasis homoeostasis
homogenizing homogenising
honor honour
honorable honourable
humor humour
hunting lodge hunting-lodge
iconoclastic iconaclastic
immobilized immobilised
immortalize immortalise
immunizing immunising
impanel empanel
incase encase
industrialization industrialisation
inflection inflexion
inquiry enquiry
ionization ionisation
ionized ionised
isomerizing isomerising
jail gaol
jail goal
jeweler jeweller
jewelry jewellery
jewlry jewellery
judgment judgement
katathermometer catathermometer
key quay
kidnaped kidnapped
kilometer kilometre
kit bag kitbag
knuckle-duster knuckleduster
labeling labelling
labor labour
laborer labourer
land mine landmine
leaned leant
leaped leapt
lemongrass lemon grass
leukemia leukaemia
leveled levelled
license licence
liter litre
localizing localising
lumber jacket lumberjacket
luster lustre
lustering lustring
M.S. M.Sc.
macadamized macadamised
magnetization magnetisation
magnetizing magnetising
magneto therapy magnetotherapy
malleableizing malleabilising
maneuver manoeuvre
maneuvering manoeuvring
market place market-place
marvelous marvellous
matchetes machets
mausoleum mauseoleum
meager meagre
medieval mediaeval
mementos mementoes
menorrhea menorrhoea
mercerizing mercerising
metacenters metacentres
metalizing metallising
meter metre
millenium millennium
mineralization mineralisation
minimizing minimising
misdemeanor misdemeanour
misjudgment misjudgement
miter mitre
modeled modelled
modeler modeller
modernize modernise
mold mould
molded moulded
molding moulding
molds moulds
moldy mouldy
mollusks molluscs
molt moult
mom mum
monolog monologue
monopolize monopolise
multicolor multicolour
mustache moustache
naught nought
nebulizers nebulisers
neighbor neighbour
neighborhood neighbourhood
neighborly neighbourly
neoclassical neo-classical
net nett
neutralization neutralisation
neutralizing neutralising
nite night
niter nitre
northeast north-east
northwest north-west
note paper note-paper
occurring occuring
octagonal octogonal
odor odour
offense offence
optimization optimisation
organization organisation
organize organise
organized organised
orthopedic orthopaedic
oxidize oxidise
oxidizing oxidising
ozonizing ozonising
pajama pyjama
paleobotany palaeobotany
paleocene palaeocene
paleoclimatology palaeoclimatology
paleogeography palaeogeography
paleography palaeography
paleolithic palaeolithic
paleomagnetism palaeomagnetism
paleontology palaeontology
paleozoic palaeozoic
paneling panelling
panelist panellist
pantyhose panti-hose
paper currency papercurrency
paralyze paralyse
parlor parlour
pasteurizing pasteurising
pavilion pavillion
pedagogy paedagogy
pedestrianize pedestrianise
pediatric paediatric
pediatrician paediatrician
pedophile paedophile
pedophilia paedophilia
phanastrons phantastrons
phase changing phasechanging
pjamas pyjamas
plasticizer plasticiser
plow plough
plowing ploughing
plowman ploughman
polarization polarisation
polarize polarise
polarized polarised
polyethylene polyethene
polymerizable polymerisable
polymerized polymerised
practicing practising
preferred prefered
preignition pre-ignition
pretense pretence
prioritize prioritise
program programme
prolog prologue
prophesy prophecy
pseudointellectual pseudo-intellectual
pseudoscience pseudo-science
pulverizing pulverising
quarreled quarrelled
raccoon racoon
railroad railway
rancor rancour
rationalize rationalise
raveled ravelled
realize realise
recognize recognise
recognizing recognising
retroflection retroflexion
revitalize revitalise
revolutionize revolutionise
rigor rigour
routing routeing
rumor rumour
saltpeter saltpetre
savior saviour
savor savour
saxophone saxaphone
scepter sceptre
selvage selvedges
sensitizing sensitising
sergeant serjeant
shop shoppe
shoveled shovelled
skeptic sceptic
skillful skilful
smolder smoulder
sniveled snivelled
snowplow snowplough
somber sombre
southeast south-east
southwest south-west
specialize specialise
specialty speciality
spelled spelt
spilled spilt
splendor splendour
stabilizer stabiliser
standardize standardise
sterilization sterilisation
storied storyed
story storey
stylized stylised
subsidizing subsidising
succor succour
sulfate sulphate
sulfide sulphide
sulfite sulphite
sulfur sulphur
sulfureted sulphuretted
sulfurous sulphurous
summarized summarised
supersede supercede
symbolize symbolise
synchronization synchronisation
synthesizing systhesising
tableting tabletting
theater theatre
tho though
thru through
tire tyre
ton tonne
toward towards
transshipment transhipment
traveled travelled
traveler traveller
traveling travelling
trisulfate trisulphate
tumor tumour
tunneling tunnelling
tyrannize tyrannise
ultrahigh ultra-high
ultramodern ultra-modern
unauthorized unauthorised
unrivaled unrivalled
unspoiled unspoilt
utilizing utilising
valor valour
vapor vapour
vaporizer vaporiser
vial phial
vigor vigour
vise vice
wagon waggon
whiskey whisky
woolen woollen
worshiped worshipped
*/

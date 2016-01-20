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

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

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
	HWND hParentWnd = NULL;
	if (!(dwStyle & MB_SYSTEMMODAL))
		hParentWnd = (m_hWndParent ? m_hWndParent : ::GetActiveWindow());

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
		RECT rMessageBox;
		if (!ComputeMessageBoxRect(rMessageBox, strMessage))
			break; // it failed, so try the old fashion way...

		CString strHtmlFile = m_strHtmlFile;
		if (strHtmlFile.IsEmpty())
			strHtmlFile = "MessageBox.htm";

		CHtmlDialog dlg(strHtmlFile, true/*bRes*/, hParentWnd);
		dlg.SetWindowSize(WIDTH(rMessageBox), HEIGHT(rMessageBox));
		dlg.SetResizable(true);
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
	Params.hwndOwner = hParentWnd;
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
bool CMessageBox::ComputeMessageBoxRect(RECT& rMessageBox, LPCTSTR pMessage)
{
	// These values should match the design driven by MessageBox.htm
	#define MSGBOX_MARGIN_L		(14+32+14)
	#define MSGBOX_MARGIN_R		(44)
	#define MSGBOX_MARGIN_T		(50+5)
	#define MSGBOX_MARGIN_B		(12+24+12+12)
	#define MSGBOX_MINMSGHEIGHT	(32)
	#define MSGBOX_FONTHEIGHT 	15 // 1.25% of the HTML message font (e.g., if 16px use 20 here)
	#define MSGBOX_FONTFACE		"arial"

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

	RECT rMargins = {
		MSGBOX_MARGIN_L + ::GetSystemMetrics(SM_CXSIZEFRAME),
		MSGBOX_MARGIN_T + ::GetSystemMetrics(SM_CYSIZEFRAME) + ::GetSystemMetrics(SM_CYCAPTION),
		MSGBOX_MARGIN_R + ::GetSystemMetrics(SM_CXSIZEFRAME),
		MSGBOX_MARGIN_B + ::GetSystemMetrics(SM_CYSIZEFRAME),
	};
	int nMaxWidth = ::GetSystemMetrics(SM_CXSCREEN) / 2;
	int nMaxHeight = nMaxWidth;
	nMaxWidth -= (rMargins.left + rMargins.right);
	nMaxHeight -= (rMargins.top + rMargins.bottom);

	RECT rMessage;
	::SetRect(&rMessage, 0, 0, nMaxWidth, nMaxHeight);
	::DrawText(hDC, pMessage, -1, &rMessage, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);

	if (HEIGHT(rMessage) < MSGBOX_MINMSGHEIGHT)
		rMessage.bottom = rMessage.top + MSGBOX_MINMSGHEIGHT;

	::SetRect(&rMessageBox, 0, 0,
		WIDTH(rMessage) + (rMargins.left + rMargins.right),
		HEIGHT(rMessage) + (rMargins.top + rMargins.bottom));

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

// AMERICAN abridgment accouterment acknowledgment advertize afterward agonize airplane almshouse aluminum ambiance ameba amphitheater analog analyze analyzed analyzer analyzing anemia anemic anesthesia anesthetic anesthetizing annex anodizing antiaircraft apologize apothegm arbor archeology archiepiscopal ardor armor armored armorer armoring armory artifact atchoo atomizer atomizing ax B.S. backward baptize barbecue bark battle-ax behavior behavioral behoove Benedictine beside bisulfate bookkeeper boro bylaw caesarean caldron caliber caliper calisthenics canceling candor carbonization carburetor carburetor carburizing carcass catalog catalyze cauterization cauterizing center centerboard centerfold centering centimeter centralize cesarean cesium channeled characterize check checker chili Christianized cigaret citrus civilization civilize clamor clangor collectible colonnaded color colored colorful coloring colter commission complete conjuror counselor cozies cozy criticize cross-linking crystallizing curb cyclization cyclopedia daydream decarburizing decolorizing defense deflexion demeanor deodorizing depolarizing depolymerizing desensitizer desulfurizing dialling dialog dialyze diarrhea dike disk disulfide dolor donut doodad draft drypoint economizer edema emphasize employe enameled enameling enamor enclosure encyclopedia endeavor enology enroll enrollment ensorcell ensure eon equaled equalizing esophageal esophagoscope esophagus estrous estrus etiology eulogize fairy tale fantasize favor favorite favoritism fecal fertilization fertilizing fertlizer fervor fetus fiber fiberglass fibers flavor flavoring fledgling flower girl flower pot fluidized fluidizing forward fresh water fulfill fulfillment fulfll furor gage galoshes galvanizing goiter gonorrhea gram graphitizing gray greenhouse groins gynecology harbor harmonize hauler hematein hemoglobin hemophilia hemorrhage hemorrhage hemorrhoid heyday homeopath homeostasis homogenizing honor honorable humor hunting lodge iconoclastic immobilized immortalize immunizing impanel incase industrialization inflection inquiry ionization ionized isomerizing jail jail jeweler jewelry jewlry judgment katathermometer key kidnaped kilometer kit bag knuckle-duster labeling labor laborer land mine leaned leaped lemongrass leukemia leveled license liter localizing lumber jacket luster lustering M.S. macadamized magnetization magnetizing magneto therapy malleableizing maneuver maneuvering market place marvelous matchetes mausoleum meager medieval mementos menorrhea mercerizing metacenters metalizing meter millenium mineralization minimizing misdemeanor misjudgment miter modeled modeler modernize mold molded molding molds moldy mollusks molt mom monolog monopolize multicolor mustache naught nebulizers neighbor neighborhood neighborly neoclassical net neutralization neutralizing nite niter northeast northwest note paper occurring octagonal odor offense optimization organization organize organized orthopedic oxidize oxidizing ozonizing pajama paleobotany paleocene paleoclimatology paleogeography paleography paleolithic paleomagnetism paleontology paleozoic paneling panelist pantyhose paper currency paralyze parlor pasteurizing pavilion pedagogy pedestrianize pediatric pediatrician pedophile pedophilia phanastrons phase changing pjamas plasticizer plow plowing plowman polarization polarize polarized polyethylene polymerizable polymerized practicing preferred preignition pretense prioritize program prolog prophesy pseudointellectual pseudoscience pulverizing quarreled raccoon railroad rancor rationalize raveled realize recognize recognizing retroflection revitalize revolutionize rigor routing rumor saltpeter savior savor saxophone scepter selvage sensitizing sergeant shop shoveled skeptic skillful smolder sniveled snowplow somber southeast southwest specialize specialty spelled spilled splendor stabilizer standardize sterilization storied story stylized subsidizing succor sulfate sulfide sulfite sulfur sulfureted sulfurous summarized supersede symbolize synchronization synthesizing tableting theater tho thru tire ton toward transshipment traveled traveler traveling trisulfate tumor tunneling tyrannize ultrahigh ultramodern unauthorized unrivaled unspoiled utilizing valor vapor vaporizer vial vigor vise wagon whiskey woolen worshiped 
// ENGLISH abridgement accoutrement acknowledgement advertise afterwards agonise aeroplane almhouse aluminium ambience amoeba amphitheatre analogue analyse analysed analyser analysing anaemia anaemic anaesthesia anaesthetic anaesthetising annexe anodising anti-aircraft apologise apophthegm arbour archaeology archipiscopal ardour armour armoured armourer armouring armoury artefact atishoo atomiser atomising axe B.Sc. backwards baptise barbeque barque battleaxe behaviour behavioural behove Benedectine besides bisulphate book-keeper borough bye law caesarian cauldron calibre calliper callisthenics cancelling candour carbonisation carburetter carburettor carburising carcas catalogue catalyse cauterisation cauterising centre centreboard centrefold centring centimetre centralise cesarian caesium channelled characterise cheque chequer chilli Christianised cigarette citrous civilisation civilise clamour clangour collectable collonaded colour coloured colourful colouring coulter commision compleat conjurer counsellor cosies cosy criticise crosslinking crystallising kerb cyclisation cyclopaedia day-dream decarburising decolourising defence deflection demeanour deodorising depolarising depolymerising desensitiser desulfurising dialing dialogue dialyse diarrhoea dyke disc disulphide dolour doughnut doodah draught dry-point economiser oedema emphasise employee enamelled enamelling enamour inclosure encyclopaedia endeavour oenology enrol enrolment ensorcel insure aeon equalled equalising oesophagal oesophagoscope oesophagus oestrous oestrous aetiology eulogise fairy-tale fantasise favour favourite favouritism faecal fertilisation fertilising fertiliser fervour foetus fibre fibreglass fibres flavour flavouring fledgeling flower-girl flower-pot fluidised fluidising forwards freshwater fulfil fulfilment fulfil furore gauge goloshes galvanising goitre gonorrhoea gramme graphitising grey glasshouse groynes gynaecology harbour harmonise haulier haematein haemoglobin haemophilia haemorrage haemorrhage haemorrhoid heydey homoeopath homoeostasis homogenising honour honourable humour hunting-lodge iconaclastic immobilised immortalise immunising empanel encase industrialisation inflexion enquiry ionisation ionised isomerising gaol goal jeweller jewellery jewellery judgement catathermometer quay kidnapped kilometre kitbag knuckleduster labelling labour labourer landmine leant leapt lemon grass leukaemia levelled licence litre localising lumberjacket lustre lustring M.Sc. macadamised magnetisation magnetising magnetotherapy malleabilising manoeuvre manoeuvring market-place marvellous machets mauseoleum meagre mediaeval mementoes menorrhoea mercerising metacentres metallising metre millennium mineralisation minimising misdemeanour misjudgement mitre modelled modeller modernise mould moulded moulding moulds mouldy molluscs moult mum monologue monopolise multicolour moustache nought nebulisers neighbour neighbourhood neighbourly neo-classical nett neutralisation neutralising night nitre north-east north-west note-paper occuring octogonal odour offence optimisation organisation organise organised orthopaedic oxidise oxidising ozonising pyjama palaeobotany palaeocene palaeoclimatology palaeogeography palaeography palaeolithic palaeomagnetism palaeontology palaeozoic panelling panellist panti-hose papercurrency paralyse parlour pasteurising pavillion paedagogy pedestrianise paediatric paediatrician paedophile paedophilia phantastrons phasechanging pyjamas plasticiser plough ploughing ploughman polarisation polarise polarised polyethene polymerisable polymerised practising prefered pre-ignition pretence prioritise programme prologue prophecy pseudo-intellectual pseudo-science pulverising quarrelled racoon railway rancour rationalise ravelled realise recognise recognising retroflexion revitalise revolutionise rigour routeing rumour saltpetre saviour savour saxaphone sceptre selvedges sensitising serjeant shoppe shovelled sceptic skilful smoulder snivelled snowplough sombre south-east south-west specialise speciality spelt spilt splendour stabiliser standardise sterilisation storyed storey stylised subsidising succour sulphate sulphide sulphite sulphur sulphuretted sulphurous summarised supercede symbolise synchronisation systhesising tabletting theatre though through tyre tonne towards transhipment travelled traveller travelling trisulphate tumour tunnelling tyrannise ultra-high ultra-modern unauthorised unrivalled unspoilt utilising valour vapour vaporiser phial vigour vice waggon whisky woollen worshipped 

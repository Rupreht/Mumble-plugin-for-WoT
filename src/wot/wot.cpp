/*
	Plugin for Game "World Of Tanks" v0.6.7
	(Made by MrRupreht & Magnum72 For clan ERMAK ;)
	www: http://rupreht.ru

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	
*/

#include "../mumble_plugin_win32.h"

static int fetch(float *avatar_pos, float *avatar_front, float *avatar_top, float *camera_pos, float *camera_front, float *camera_top, std::string &context, std::wstring &identity) {

	// Zeroing the floats
	for (int i=0;i<3;i++)
		avatar_pos[i]=avatar_front[i]=avatar_top[i]=camera_pos[i]=camera_front[i]=camera_top[i]=0.0f;

	char state;
	bool ok;

	ok = peekProc((BYTE *) 0x00F918B8, &state, 1); // Magical state value
	if (! ok)
		return false;

	if (state == 0)
		return true; // This results in all vectors beeing zero which tells Mumble to ignore them.
	identity = L"in Game";
	// coordinate systems:
	// "ENU"	"Mumble"	"In game"
	// East		x+ [0]		x+ [0]
	// Up		y+ [1]		y+ [1]
	// North	z+ [2]		z+ [2]
	
	float pos_corrector[3];
	float front_corrector[2];

	// Peekproc and assign game addresses to our containers, so we can retrieve positional data
	ok = peekProc((BYTE *) 0x00FD0B34, &pos_corrector, 12) &&
		peekProc((BYTE *) 0x00FD0CDC, &front_corrector, 8);

	if (! ok)
		return false;

	// Convert to left-handed coordinate system

	avatar_pos[0] = pos_corrector[0];
	avatar_pos[1] = pos_corrector[1];
	avatar_pos[2] = pos_corrector[2];

	avatar_front[0] = front_corrector[0];
	avatar_front[1] = 0;
	avatar_front[2] = front_corrector[1];

	for (int i=0;i<3;i++) {
		camera_pos[i] = avatar_pos[i];
		camera_front[i] = avatar_front[i];
		camera_top[i] = avatar_top[i];
	}

	return true;
}

static int trylock(const std::multimap<std::wstring, unsigned long long int> &pids) {

	if ((! initialize(pids, L"WorldOfTanks.exe", L"worldoftanks.exe")) && (! initialize(pids, L"WorldOfTanks.exe")))
        return false;

		// Check if we can get meaningful data from it
		float apos[3], afront[3], atop[3];
		float cpos[3], cfront[3], ctop[3];
		std::wstring sidentity;
		std::string scontext;

		if (fetch(apos, afront, atop, cpos, cfront, ctop, scontext, sidentity)) {
			return true;
		} else {
			generic_unlock();
			return false;
		}
}

static const std::wstring longdesc() {
        return std::wstring( L"Supports World Of Tanks v0.6.7. No identity support yet.");
}

static std::wstring description( L"World Of Tanks v0.6.7 (Made by Rupreht & Magnum72 For clan ERMAK ;)");
static std::wstring shortname( L"World Of Tanks");

static int trylock1() {
	return trylock(std::multimap<std::wstring, unsigned long long int>());
}

static MumblePlugin wotplug = {
	MUMBLE_PLUGIN_MAGIC,
	description,
	shortname,
	NULL,
	NULL,
	trylock1,
	generic_unlock,
	longdesc,
	fetch
};

static MumblePlugin2 wotplug2 = {
	MUMBLE_PLUGIN_MAGIC_2,
	MUMBLE_PLUGIN_VERSION,
	trylock
};

extern "C" __declspec(dllexport) MumblePlugin *getMumblePlugin() {
	return &wotplug;
}

extern "C" __declspec(dllexport) MumblePlugin2 *getMumblePlugin2() {
	return &wotplug2;
}
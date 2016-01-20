package {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.net.URLRequest;
	
	import net.quip.sound.CuePointEvent;
	import net.quip.sound.SoundSync;

	public class SoundSyncExample extends Sprite
	{
		// CONSTRUCTOR
		public function SoundSyncExample()
		{
			var ss:SoundSync = new SoundSync();
			ss.load(new URLRequest("green_presidents.mp3"));
			ss.addCuePoint("George Washington", 2877);
			ss.addCuePoint("Thomas Jefferson", 6000);
			ss.addCuePoint("Abraham Lincoln", 9000);
			ss.addCuePoint("Alexander Hamilton", 12326);
			ss.addCuePoint("Andrew Jackson", 15336);
			ss.addCuePoint("Ulysses S. Grant", 18000);
			ss.play();
			ss.addEventListener(CuePointEvent.CUE_POINT, onCuePoint);
			ss.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
		}
		// On Cue Point
		private function onCuePoint(event:CuePointEvent):void {
			trace("Cue point: " + event.name + ", " + event.time);
		}
		// On Sound Complete
		private function onSoundComplete(event:Event):void {
			trace("Cue point: " + event.type);
		}
	}
}

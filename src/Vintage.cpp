#include "plugin.hpp"

struct Vintage : Module {
	enum ParamIds {
		ENUMS(PARAM2, 4),
		ENUMS(PARAM3, 4),
		ENUMS(PARAM4, 4),
		ENUMS(PARAM5, 4),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(IN_INPUT, 2),
		ENUMS(CV_INPUT, 2),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUTPUTS, 10),
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Vintage() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
			configParam(PARAM2 + 0, -12.f, 12.f, -5.f, "inmin");
			configParam(PARAM3 + 0, -12.f, 12.f, 5.f, "inmax");
			configParam(PARAM2 + 2, -12.f, 12.f, -5.f, "inmin");
			configParam(PARAM3 + 2, -12.f, 12.f, 5.f, "inmax");
			
			configParam(PARAM2 + 1, -12.f, 12.f, -5.f, "outmin");
			configParam(PARAM3 + 1, -12.f, 12.f, 5.f, "outmax");
			configParam(PARAM2 + 3, -12.f, 12.f, -5.f, "outmin");
			configParam(PARAM3 + 3, -12.f, 12.f, 5.f, "outmax");
			
			configParam(PARAM4 + 0, 1.f, 12.f, 1.f, "expdiv");
			configParam(PARAM5 + 0, 0.f, 12.f, 4.f, "expmul");
			configParam(PARAM4 + 1, 1.f, 12.f, 1.f, "logdiv");
			configParam(PARAM5 + 1, -1.f, 12.f, 10.f, "logmul");
			
			configParam(PARAM4 + 2, 1.f, 12.f, 1.f, "expdiv");
			configParam(PARAM5 + 2, 0.f, 12.f, 1.f, "expmul");
			configParam(PARAM4 + 3, 1.f, 12.f, 1.f, "logdiv");
			configParam(PARAM5 + 3, -1.f, 12.f, 0.f, "logmul");
	  /*
	  configInput(IN_INPUT+0, "CV");
	  configInput(IN_INPUT+1, "CV");
	  configInput(CV_INPUT+0, "IN");
	  configInput(CV_INPUT+1, "IN");
	  
	  configOutput(OUTPUTS+0, "EXP");
	  configOutput(OUTPUTS+1, "LOG");
	  configOutput(OUTPUTS+2, "EXP");
	  configOutput(OUTPUTS+3, "LOG");
	  configOutput(OUTPUTS+4, "EXPI");
	  configOutput(OUTPUTS+5, "LOGI");
	  configOutput(OUTPUTS+6, "EXPI");
	  configOutput(OUTPUTS+7, "LOGI");
	  configOutput(OUTPUTS+8, "SINE");
	  configOutput(OUTPUTS+9, "SINE"); */
	}

	void process(const ProcessArgs& args) override {
		int channels = inputs[CV_INPUT].getChannels();
		int channel2 = inputs[CV_INPUT+1].getChannels();
		
		outputs[OUTPUTS+0].setChannels(channels);
		outputs[OUTPUTS+4].setChannels(channels);
		outputs[OUTPUTS+1].setChannels(channels);
		outputs[OUTPUTS+5].setChannels(channels);
		outputs[OUTPUTS+8].setChannels(channels);
		
		outputs[OUTPUTS+2].setChannels(channel2);
		outputs[OUTPUTS+6].setChannels(channel2);
		outputs[OUTPUTS+3].setChannels(channel2);
		outputs[OUTPUTS+7].setChannels(channel2);
		outputs[OUTPUTS+9].setChannels(channel2);

		float param5 = params[PARAM5].getValue();
		float param52 = params[PARAM5+1].getValue();
		if (inputs[IN_INPUT].isConnected()) {
			param52 = clamp(inputs[IN_INPUT].getVoltage(),-1.f,12.f);
			param5 = clamp(inputs[IN_INPUT].getVoltage(),0.f,12.f);
		}
			
		for (int c = 0; c < channels; c++) {
			float indif = params[PARAM3].getValue()-params[PARAM2].getValue();
			float minor = indif < 0 ? params[PARAM3].getValue() : params[PARAM2].getValue();
			float cvin = clamp((inputs[CV_INPUT].getVoltage(c)-minor)/std::abs(indif), 0.f, 1.f);
			
			float indife = params[PARAM3+1].getValue()-params[PARAM2+1].getValue();
			float minore = indife < 0 ? params[PARAM3+1].getValue() : params[PARAM2+1].getValue();
			if (indif*indife < 0) cvin = 1.f-cvin;
			
			float output1 = std::pow(cvin, param5/params[PARAM4].getValue());
			float output2 = std::exp(std::log(cvin)*params[PARAM4].getValue()/param5);
			float output3 = (cvin*param52/params[PARAM4+1].getValue()+cvin)/(cvin*param52/params[PARAM4+1].getValue()+1);
			float output4 = cvin/(-cvin*param52/params[PARAM4+1].getValue()+param52/params[PARAM4+1].getValue()+1);
			float output5 = 0.5f+std::cos(M_PI*(1.f-cvin))/2.f;
			
			outputs[OUTPUTS+0].setVoltage(output1*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+4].setVoltage(output2*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+1].setVoltage(output3*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+5].setVoltage(output4*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+8].setVoltage(output5*
			std::abs(indife)+minore,c);
		}		

		param5 = params[PARAM5+2].getValue();
		param52 = params[PARAM5+3].getValue();
		if (inputs[IN_INPUT+1].isConnected()) {
			param52 = clamp(inputs[IN_INPUT+1].getVoltage(),-1.f,12.f);
			param5 = clamp(inputs[IN_INPUT+1].getVoltage(),0.f,12.f);
		}
		
		for (int c = 0; c < channel2; c++) {
			float indif = params[PARAM3+2].getValue()-params[PARAM2+2].getValue();
			float minor = indif < 0 ? params[PARAM3+2].getValue() : params[PARAM2+2].getValue();
			float cvin = clamp((inputs[CV_INPUT+1].getVoltage(c)-minor)/std::abs(indif), 0.f, 1.f);

			float indife = params[PARAM3+3].getValue()-params[PARAM2+3].getValue();
			float minore = indife < 0 ? params[PARAM3+3].getValue() : params[PARAM2+3].getValue();
			if (indif*indife < 0) cvin = 1.f-cvin;

			float output1 = std::pow(cvin, param5/params[PARAM4+2].getValue());
			float output2 = std::exp(std::log(cvin)*params[PARAM4+2].getValue()/param5);
			float output3 = (cvin*param52/params[PARAM4+3].getValue()+cvin)/(cvin*param52/params[PARAM4+3].getValue()+1);
			float output4 = cvin/(-cvin*param52/params[PARAM4+3].getValue()+param52/params[PARAM4+3].getValue()+1);
			float output5 = 0.5f+std::cos(M_PI*(1.f-cvin))/2.f;

			outputs[OUTPUTS+2].setVoltage(output1*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+6].setVoltage(output2*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+3].setVoltage(output3*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+7].setVoltage(output4*
			std::abs(indife)+minore,c);
			outputs[OUTPUTS+9].setVoltage(output5*
			std::abs(indife)+minore,c);
		}
	}
};


struct VintageWidget : ModuleWidget {
	VintageWidget(Vintage* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Vintage.svg")));

      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*1+6.425)), module, Vintage::PARAM4 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*1+6.425)), module, Vintage::PARAM4 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*1+6.425)), module, Vintage::PARAM4 + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*4, 12.85*1+6.425)), module, Vintage::PARAM4 + 3));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*2+6.425)), module, Vintage::PARAM5 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*2+6.425)), module, Vintage::PARAM5 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*2+6.425)), module, Vintage::PARAM5 + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*4, 12.85*2+6.425)), module, Vintage::PARAM5 + 3));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*3+6.425)), module, Vintage::OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*3+6.425)), module, Vintage::OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*3+6.425)), module, Vintage::OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*3+6.425)), module, Vintage::OUTPUTS + 3));
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*4+6.425)), module, Vintage::OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*4+6.425)), module, Vintage::OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*4+6.425)), module, Vintage::OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*4+6.425)), module, Vintage::OUTPUTS + 7));
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*5+6.425)), module, Vintage::PARAM2 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*5+6.425)), module, Vintage::PARAM3 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*5+6.425)), module, Vintage::PARAM2 + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*4, 12.85*5+6.425)), module, Vintage::PARAM3 + 3));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*6+6.425)), module, Vintage::CV_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*6+6.425)), module, Vintage::IN_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*6+6.425)), module, Vintage::CV_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*6+6.425)), module, Vintage::IN_INPUT + 1));
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*7+6.425)), module, Vintage::PARAM2 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*8+6.425)), module, Vintage::PARAM3 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*7+6.425)), module, Vintage::PARAM2 + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*8+6.425)), module, Vintage::PARAM3 + 2));
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*8+6.425)), module, Vintage::OUTPUTS + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*8+6.425)), module, Vintage::OUTPUTS + 9));
		
	}

};

Model* modelVintage = createModel<Vintage, VintageWidget>("Vintage");
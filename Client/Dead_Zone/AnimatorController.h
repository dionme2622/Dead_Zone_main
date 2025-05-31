// AnimatorController.h
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "AnimatorParameter.h"
#include "AnimationState.h"

class AnimatorController
{
public:
    void AddParameter(const AnimatorParameter& param)
    {
        int idx = (int)_paramDefs.size();
        _paramMap[param.name] = idx;
        _paramDefs.push_back(param);
    }

    void AddState(shared_ptr<AnimationState> state)
    {
        _states[state->GetName()] = state;
    }

    void SetEntryState(const wstring& name)
    {
        _entryState = _states[name];
        _currentState = _entryState;
    }

    void SetAnyState(const wstring& name)
    {
        _anyState = _states[name];
    }

    void SetCurrentState(const wstring& name)
    {
        _currentState = _states[name];
    }
    shared_ptr<AnimationState> GetCurrentState() const { return _currentState; }
    shared_ptr<AnimationState> GetEntryState() const { return _entryState; }
    shared_ptr<AnimationState> GetAnyState() const { return _anyState; }

    const vector<AnimatorParameter>& GetParamDefs() const { return _paramDefs; }
    int GetParamIndex(const string& name) const
    {
        auto it = _paramMap.find(name);
        return it == _paramMap.end() ? -1 : it->second;
    }

    shared_ptr<AnimatorController> Clone() const {
        // 1) 새 컨트롤러 인스턴스 생성
        auto copy = std::make_shared<AnimatorController>();

        // 2) 파라미터 정의 복사
        copy->_paramDefs = _paramDefs;
        copy->_paramMap = _paramMap;

        // 3) 스테이트 머신 정의(스테이트 포인터) 복사
        //    – AnimationState 객체는 "정의(transition, clip 등)"만 담고 있으므로
        //      얕은 복사로 공유해도 안전합니다.
        for (auto& kv : _states)
        {
            copy->_states[kv.first] = kv.second;
        }

        // 4) 엔트리, any, current 스테이트 복제본에서 다시 찾아 연결
        if (_entryState)
            copy->_entryState = copy->_states.at(_entryState->GetName());
        if (_anyState)
            copy->_anyState = copy->_states.at(_anyState->GetName());
        if (_currentState)
            copy->_currentState = copy->_states.at(_currentState->GetName());

        return copy;


    }

private:
    vector<AnimatorParameter> _paramDefs;
    unordered_map<string, int> _paramMap;

    unordered_map<wstring, shared_ptr<AnimationState>> _states;
    shared_ptr<AnimationState> _entryState;
    shared_ptr<AnimationState> _anyState;
    shared_ptr<AnimationState> _currentState;
};
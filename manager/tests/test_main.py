##########################################################################################
#   永続的に状態管理を行うAPIのテストコードです。
##########################################################################################
import pytest
import api.main as main

def setup_module(module):
    """ユニットテストではIFTTTを強制的に無効化します。
    """
    main.ENABLED_IFTTT = False

@pytest.mark.dependency()
def test_reset():
    assert isinstance(main.reset(), dict)

@pytest.mark.dependency(depends=["test_reset"])
def test_turn1():
    assert isinstance(main.turn({
        "id": 101,
    }), dict)

@pytest.mark.dependency(depends=["test_turn1"])
def test_turn2():
    assert isinstance(main.turn({
        "id": 101,
    }), dict)

@pytest.mark.dependency(depends=["test_reset"])
def test_get_one():
    result = main.get({
        "id": 101,
    })
    assert type(result) == str
    assert type(int(result)) == int

@pytest.mark.dependency(depends=["test_reset"])
def test_get_all():
    result = main.get({})
    assert type(result) == str
    assert type(int(result)) == int

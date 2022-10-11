library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Automat is
	port( 
			-- Reset
			SW1: in std_logic;
			-- Start / Nowa gra
			SW2: in std_logic;
			-- Stop dobierania
			SW3: in std_logic;
			-- Dobierz
			SW4: in std_logic;

			CLK: in std_logic;
			Q: out std_logic_vector(2 downto 0));
end Automat;

architecture Behavioral of Automat is
signal Stan: std_logic_vector(2 downto 0):= "011";
begin
	process(CLK)
	begin	
		if CLK'event and CLK = '1' then
-- =========================================
-- 				STAN RESET
-- =========================================
			if Stan = "000" then
				-- Guzik reset
				if SW1 = '1' then
					Stan <= "000";
				else
					Stan <= "001";
				end if;
			end if;
-- =========================================
-- 				STAN GRACZ 1
-- =========================================
			if Stan = "001" then
				-- Guzik reset
				if SW1 = '1' then
					Stan <= "000";
				end if;
				
				-- Guzik stop dobierania
				if SW3 = '1' then
					Stan <= "111";
				end if;
				
				-- Guzik dobierz karte
				if SW4 = '1' then
					Stan <= "100";
				end if;
			end if;
-- =========================================
-- 				STAN DOBIERZ G1
-- =========================================
			if Stan = "100" then
				if SW4 = '0' then
					Stan <= "001";
				end if;
			end if;
-- =========================================
-- 				STAN WPIS 1
-- =========================================
			if Stan = "111" then
				if SW3 = '0' then
					Stan <= "010";
				end if;
			end if;
-- =========================================
-- 				STAN GRACZ 2
-- =========================================
			if Stan = "010" then
				-- Guzik reset
				if SW1 = '1' then
					Stan <= "000";
				end if;
				
				-- Guzik stop dobierania
				if SW3 = '1' then
					Stan <= "110";
				end if;
				
				-- Guzik dobierz karte
				if SW4 = '1' then
					Stan <= "101";
				end if;
			end if;
-- =========================================
-- 				STAN DOBIERZ G2
-- =========================================
			if Stan = "101" then
				if SW4 = '0' then
					Stan <= "010";
				end if;
			end if;
-- =========================================
-- 				STAN WPIS 2
-- =========================================
			if Stan = "110" then
				if SW4 = '0' then
					Stan <= "011";
				end if;
			end if;
-- =========================================
-- 				STAN WYNIKI
-- =========================================
			if Stan = "011" then
				-- Guzik reset
				if SW1 = '1' then
					Stan <= "000";
				end if;
				
				-- Guzik start gry
				if SW2 = '1' then
					Stan <= "001";
				end if;
			end if;
-- =========================================
		end if;
	end process;
	
	Q <= Stan;
	
end Behavioral;

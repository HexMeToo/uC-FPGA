library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Sterowanie is
	port(	CLK: in std_logic;
			Wynik: in std_logic_vector(4 downto 0);
			A: in std_logic_vector(2 downto 0);
			R: out std_logic;
			Syg_D: buffer std_logic;
			W1: buffer std_logic_vector(3 downto 0);
			W2: buffer std_logic_vector(3 downto 0)
			);
end Sterowanie;

architecture Behavioral of Sterowanie is
signal Gracz1: std_logic_vector(4 downto 0) := "00000";
signal Gracz2: std_logic_vector(4 downto 0) := "00000";
begin
	process(A, CLK)
	begin
		if clk'event and clk = '1' then
			case A is
-- =========================================
-- 				STAN RESET
-- =========================================
				when "000" =>
					R <= '1';
-- =========================================
-- 				STAN GRACZ 1
-- =========================================
				when "001" =>
					Syg_D <= '0';
					W1 <= "1010";
					W2 <= "0001";
					R <= '0';
					Gracz1 <= Wynik;
-- =========================================
-- 				STAN DOBIERZ G1
-- =========================================
				when "100" =>
					if Wynik < 21 then
						Syg_D <= '1';
					end if;
-- =========================================
-- 				STAN WPIS 1
-- =========================================
				when "111" =>
					if Gracz1 > 21 then
						Gracz1 <= "00000";
					end if;
					R <= '1';
-- =========================================
-- 				STAN GRACZ 2
-- =========================================
				when "010" =>
					Syg_D <= '0';
					W1 <= "1010";
					W2 <= "0010";
					R <= '0';
					Gracz2 <= Wynik;
-- =========================================
-- 				STAN DOBIERZ G2
-- =========================================
				when "101" =>
					if Wynik < 21 then
						Syg_D <= '1';
					end if;
-- =========================================
-- 				STAN WPIS 2
-- =========================================
				when "110" =>
					if Gracz2 > 21 then
						Gracz2 <= "00000";
					end if;
					R <= '1';
-- =========================================
-- 				STAN WYNIKI
-- =========================================
				when "011" =>
					if Gracz1 > Gracz2 then
						W1 <= "1010";
						W2 <= "0001";
					end if;
					if Gracz1 < Gracz2 then
						W1 <= "1010";
						W2 <= "0010";
					end if;
					if Gracz1 = Gracz2 then
						W1 <= "1010";
						W2 <= "1010";
					end if;
-- ==============================================
			end case;			
		end if;
	end process;
end Behavioral;